


(define (problem hanoi-19)
(:domain hanoi)
(:objects peg1 peg2 peg3 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15 d16 d17 d18 d19 )
(:init
(smaller peg1 d1)
(smaller peg1 d2)
(smaller peg1 d3)
(smaller peg1 d4)
(smaller peg1 d5)
(smaller peg1 d6)
(smaller peg1 d7)
(smaller peg1 d8)
(smaller peg1 d9)
(smaller peg1 d10)
(smaller peg1 d11)
(smaller peg1 d12)
(smaller peg1 d13)
(smaller peg1 d14)
(smaller peg1 d15)
(smaller peg1 d16)
(smaller peg1 d17)
(smaller peg1 d18)
(smaller peg1 d19)
(smaller peg2 d1)
(smaller peg2 d2)
(smaller peg2 d3)
(smaller peg2 d4)
(smaller peg2 d5)
(smaller peg2 d6)
(smaller peg2 d7)
(smaller peg2 d8)
(smaller peg2 d9)
(smaller peg2 d10)
(smaller peg2 d11)
(smaller peg2 d12)
(smaller peg2 d13)
(smaller peg2 d14)
(smaller peg2 d15)
(smaller peg2 d16)
(smaller peg2 d17)
(smaller peg2 d18)
(smaller peg2 d19)
(smaller peg3 d1)
(smaller peg3 d2)
(smaller peg3 d3)
(smaller peg3 d4)
(smaller peg3 d5)
(smaller peg3 d6)
(smaller peg3 d7)
(smaller peg3 d8)
(smaller peg3 d9)
(smaller peg3 d10)
(smaller peg3 d11)
(smaller peg3 d12)
(smaller peg3 d13)
(smaller peg3 d14)
(smaller peg3 d15)
(smaller peg3 d16)
(smaller peg3 d17)
(smaller peg3 d18)
(smaller peg3 d19)
(smaller d1 d1)
(smaller d2 d1)
(smaller d3 d1)
(smaller d4 d1)
(smaller d5 d1)
(smaller d6 d1)
(smaller d7 d1)
(smaller d8 d1)
(smaller d9 d1)
(smaller d10 d1)
(smaller d11 d1)
(smaller d12 d1)
(smaller d13 d1)
(smaller d14 d1)
(smaller d15 d1)
(smaller d16 d1)
(smaller d17 d1)
(smaller d18 d1)
(smaller d19 d1)
(smaller d2 d2)
(smaller d3 d2)
(smaller d4 d2)
(smaller d5 d2)
(smaller d6 d2)
(smaller d7 d2)
(smaller d8 d2)
(smaller d9 d2)
(smaller d10 d2)
(smaller d11 d2)
(smaller d12 d2)
(smaller d13 d2)
(smaller d14 d2)
(smaller d15 d2)
(smaller d16 d2)
(smaller d17 d2)
(smaller d18 d2)
(smaller d19 d2)
(smaller d3 d3)
(smaller d4 d3)
(smaller d5 d3)
(smaller d6 d3)
(smaller d7 d3)
(smaller d8 d3)
(smaller d9 d3)
(smaller d10 d3)
(smaller d11 d3)
(smaller d12 d3)
(smaller d13 d3)
(smaller d14 d3)
(smaller d15 d3)
(smaller d16 d3)
(smaller d17 d3)
(smaller d18 d3)
(smaller d19 d3)
(smaller d4 d4)
(smaller d5 d4)
(smaller d6 d4)
(smaller d7 d4)
(smaller d8 d4)
(smaller d9 d4)
(smaller d10 d4)
(smaller d11 d4)
(smaller d12 d4)
(smaller d13 d4)
(smaller d14 d4)
(smaller d15 d4)
(smaller d16 d4)
(smaller d17 d4)
(smaller d18 d4)
(smaller d19 d4)
(smaller d5 d5)
(smaller d6 d5)
(smaller d7 d5)
(smaller d8 d5)
(smaller d9 d5)
(smaller d10 d5)
(smaller d11 d5)
(smaller d12 d5)
(smaller d13 d5)
(smaller d14 d5)
(smaller d15 d5)
(smaller d16 d5)
(smaller d17 d5)
(smaller d18 d5)
(smaller d19 d5)
(smaller d6 d6)
(smaller d7 d6)
(smaller d8 d6)
(smaller d9 d6)
(smaller d10 d6)
(smaller d11 d6)
(smaller d12 d6)
(smaller d13 d6)
(smaller d14 d6)
(smaller d15 d6)
(smaller d16 d6)
(smaller d17 d6)
(smaller d18 d6)
(smaller d19 d6)
(smaller d7 d7)
(smaller d8 d7)
(smaller d9 d7)
(smaller d10 d7)
(smaller d11 d7)
(smaller d12 d7)
(smaller d13 d7)
(smaller d14 d7)
(smaller d15 d7)
(smaller d16 d7)
(smaller d17 d7)
(smaller d18 d7)
(smaller d19 d7)
(smaller d8 d8)
(smaller d9 d8)
(smaller d10 d8)
(smaller d11 d8)
(smaller d12 d8)
(smaller d13 d8)
(smaller d14 d8)
(smaller d15 d8)
(smaller d16 d8)
(smaller d17 d8)
(smaller d18 d8)
(smaller d19 d8)
(smaller d9 d9)
(smaller d10 d9)
(smaller d11 d9)
(smaller d12 d9)
(smaller d13 d9)
(smaller d14 d9)
(smaller d15 d9)
(smaller d16 d9)
(smaller d17 d9)
(smaller d18 d9)
(smaller d19 d9)
(smaller d10 d10)
(smaller d11 d10)
(smaller d12 d10)
(smaller d13 d10)
(smaller d14 d10)
(smaller d15 d10)
(smaller d16 d10)
(smaller d17 d10)
(smaller d18 d10)
(smaller d19 d10)
(smaller d11 d11)
(smaller d12 d11)
(smaller d13 d11)
(smaller d14 d11)
(smaller d15 d11)
(smaller d16 d11)
(smaller d17 d11)
(smaller d18 d11)
(smaller d19 d11)
(smaller d12 d12)
(smaller d13 d12)
(smaller d14 d12)
(smaller d15 d12)
(smaller d16 d12)
(smaller d17 d12)
(smaller d18 d12)
(smaller d19 d12)
(smaller d13 d13)
(smaller d14 d13)
(smaller d15 d13)
(smaller d16 d13)
(smaller d17 d13)
(smaller d18 d13)
(smaller d19 d13)
(smaller d14 d14)
(smaller d15 d14)
(smaller d16 d14)
(smaller d17 d14)
(smaller d18 d14)
(smaller d19 d14)
(smaller d15 d15)
(smaller d16 d15)
(smaller d17 d15)
(smaller d18 d15)
(smaller d19 d15)
(smaller d16 d16)
(smaller d17 d16)
(smaller d18 d16)
(smaller d19 d16)
(smaller d17 d17)
(smaller d18 d17)
(smaller d19 d17)
(smaller d18 d18)
(smaller d19 d18)
(clear peg2)
            (clear peg3)
            (clear d1)
(on d19 peg1)
(on d18 d19)
(on d17 d18)
(on d16 d17)
(on d15 d16)
(on d14 d15)
(on d13 d14)
(on d12 d13)
(on d11 d12)
(on d10 d11)
(on d9 d10)
(on d8 d9)
(on d7 d8)
(on d6 d7)
(on d5 d6)
(on d4 d5)
(on d3 d4)
(on d2 d3)
(on d1 d2)
)
(:goal
(and 
(on d19 peg3)
(on d18 d19)
(on d17 d18)
(on d16 d17)
(on d15 d16)
(on d14 d15)
(on d13 d14)
(on d12 d13)
(on d11 d12)
(on d10 d11)
(on d9 d10)
(on d8 d9)
(on d7 d8)
(on d6 d7)
(on d5 d6)
(on d4 d5)
(on d3 d4)
(on d2 d3)
(on d1 d2)
)
)
)


