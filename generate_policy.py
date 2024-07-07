# python3

import torch
import os
import random

from functools import reduce

def state_hash(state):
    if isinstance(state, torch.Tensor):
        state = map(int, state.tolist())
        
    hash = 0
    for i in state:
        hash = (hash << 1) + i
    return hash

# Reads samples from a file with given filename (without file extension).
# A file with the given filename must be located in the resources/sl_samples
# directory and must have the file extension .samples .
def read_samples(file):  
    inputs = []
    outputs = []
    applicable_actions = dict()
 
    with open(os.path.join("resources", "sl_samples", f"{file}.samples"), "r") as file:
        lines = file.readlines()
        
        num_facts = len(lines[2].split(";"))
        num_ops = len(lines[3].split(";"))
                
        plan_inputs = []
        plan_outputs = []
        
        for line in lines[4:]:
            if line[0] == '#':
                if not len(plan_inputs) == 0:
                    inputs.append(plan_inputs)
                    outputs.append(plan_outputs)
                    plan_inputs = []
                    plan_outputs = []
                    
                continue            
            
            l = line.split(";")
            l = list(map(int, l))
            h_val = l[0]
            state = l[1:num_facts+1]
            applicable = list(map(lambda x: x >> 1, l[num_facts+1:]))
            operators = list(map(lambda x: x & 1, l[num_facts+1:]))
            
            assert(len(state) == num_facts)
            assert(len(applicable) == num_ops)
            assert(len(operators) == num_ops)
            
            if h_val == 0:
                continue
            
            plan_inputs.append(state)
            plan_outputs.append(operators)
            
            hash = state_hash(state)
            if hash in applicable_actions:
                assert(applicable_actions[hash] == applicable)
            else:
                applicable_actions[hash] = applicable
                        
    assert(len(inputs) == len(outputs))
            
    return inputs, outputs, applicable_actions

# Writes a model to a file with given filename (without file extension).
# The model is written to the file resources/policies/filename.pt .
def save_model_to_file(model, filename):
    path = os.path.join("resources", "policies", f"{filename}.pt")
    print(f"Saving model to file {path}...")
    s = torch.jit.script(model)
    s.save(path)

# Computes the accuracy of the model's on the validation data when interpreted 
# as a deterministic policy. 
# Returns two numbers:
#   (1) the number of correct predictions and
#   (2) the total number of samples in the validation data.
# The accuracy is the quotient of both numbers.
def compute_accuracy(model, val_inputs, val_outputs):
    with torch.no_grad():
        model.eval()
                
        y = torch.softmax(model(val_inputs), dim=1)

        predicted = torch.argmax(y, 1)
        predicted_val = torch.argmax(val_outputs, 1)
        
        correct = (predicted == predicted_val).sum()
            
        return correct, len(val_inputs)

class MaskedModule(torch.nn.Module):
    def __init__(self, base_model, applicable_actions, *args, **kwargs):
        super(MaskedModule, self).__init__(*args, **kwargs)
        self.base_model = base_model
        self.applicable_actions = applicable_actions
        
    def get_applicability_masks(self, states):
        return torch.BoolTensor([self.applicable_actions[state_hash(state)] for state in states])

    def forward(self, inputs):
        masks = self.get_applicability_masks(inputs)
        ypre = self.base_model.forward(inputs)
        return torch.where(masks, ypre, -float("inf"))

# Initialize random seeds for reproducibility
torch.use_deterministic_algorithms(True)
torch.manual_seed(42)
random.seed(42)

# Step 1.1: Read samples from the corresponding file (TODO)
# See helper function read_samples
inputs, outputs, applicable_actions = read_samples("task5")

print("======== Read data set ========")
print(f"  Total number of plans: {len(inputs)}")
print(f"  Total number of samples: {reduce(lambda x, y: x + len(y), inputs, 0)}")

# The length of a state input vector (number of facts)
# and output probability vector (number of actions)
input_width = len(inputs[0][0])     
output_width = len(outputs[0][1])   


# # Step 1.2: Split data set into training data and validation data (TODO)
# # Note: The lists of inputs and outputs must be wrapped into a float tensor so
# # that PyTorch can work with them
samples_x = []
samples_y = []
for i in range(len(inputs)):
    for j in range(len(inputs[i])):
        samples_x.append(inputs[i][j])
        samples_y.append(outputs[i][j])

split_ratio = 0.6
train_size = int(len(samples_x) * split_ratio)
val_size = len(samples_x) - train_size

inputs_train,inputs_val = torch.utils.data.random_split(samples_x, [train_size, val_size],generator=torch.Generator().manual_seed(54))
outputs_train,outputs_val = torch.utils.data.random_split(samples_y, [train_size, val_size],generator=torch.Generator().manual_seed(54))

inputs_train = torch.FloatTensor(inputs_train)
outputs_train = torch.FloatTensor(outputs_train)

inputs_val = torch.FloatTensor(inputs_val)
outputs_val = torch.FloatTensor(outputs_val)

# Step 2.1: Construct the base FFN (TODO)
base_model =  torch.nn.Sequential(
    torch.nn.Linear(input_width, 64),
     torch.nn.ReLU(),
     torch.nn.Linear(64, output_width)
)

# # Step 2.2: Wrap FFN into MaskedModule to mask outputs corresponding to 
# # inapplicable actions
model = MaskedModule(base_model, applicable_actions)

# # Step 3.1: Define loss function (TODO)
lossfn = torch.nn.CrossEntropyLoss()

# # Step 3.2: Define the optimizer (TODO)
optimizer = torch.optim.SGD(model.parameters(), lr=0.01)

# # Step 3.3: Train the FFN (TODO)
num_epochs = 200
batch_size = 64
model.train()
for i in range(0, num_epochs):
    #Sample minibatches from the training data (TODO)
    indices = torch.randperm(len(inputs_train))
    for start in range(0, len(indices), batch_size):
        end = min(start + batch_size, len(indices))
        batch_indices = indices[start:end]
        batch_inputs = inputs_train[batch_indices]
        batch_outputs = outputs_train[batch_indices]
    # For each minibatch, train the model (TODO)
        optimizer.zero_grad()
        predictions = model(batch_inputs)
        loss = lossfn(predictions, torch.argmax(batch_outputs, dim=1))
        loss.backward()
        optimizer.step()
    # Compute accuracy on validation data (TODO)
    # See helper function compute_accuracy
    # It makes sense to stop if the accuracy is very high (e.g. >= 95%)
    correct, total = compute_accuracy(model, inputs_val, outputs_val)
    accuracy = correct / total
    print(f'Epoch {i+1}/{num_epochs},  Accuracy: {accuracy}')
    if accuracy >= 0.95:
        print("Training stopped. Achieved high accuracy.")
        break
    pass

# Step 4: Append a softmax layer to the *base model* to transform its output to a probability distribution (implicit during training) (TODO)
base_model.add_module('softmax', torch.nn.Softmax(dim=-1))
# The mask for inapplicable actions will be applied during evaluation of the 
# policy, so append a basic softmax layer to the base model.
pass

# Step 5: Write the base model with appended softmax layer to an output file as 
# a script (TODO)
# See helper function save_model_to_file
save_model_to_file(base_model, "task5")
pass