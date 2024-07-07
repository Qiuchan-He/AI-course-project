#ifndef DOWNWARD_OPTIONS_REGISTRIES_H
#define DOWNWARD_OPTIONS_REGISTRIES_H

#include "downward/options/doc_utils.h"
#include "downward/options/raw_registry.h"

#include <any>
#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace options {
class OptionParser;
class Predefinitions;

struct RawPredefinition {
    RawPredefinition(
        const std::string key,
        const std::string raw,
        bool dry_run);

    const std::string key;
    const std::string raw;
    const bool dry_run;
};

class Registry {
    std::unordered_map<
        std::type_index,
        std::unordered_map<std::string, std::any>>
        plugin_factories;
    /*
      plugin_type_infos collects information about all plugin types
      in use and gives access to the underlying information. This is used,
      for example, to generate the complete help output.
    */
    std::unordered_map<std::type_index, PluginTypeInfo> plugin_type_infos;
    /*
      The plugin group registry collects information about plugin groups.
      A plugin group is a set of plugins (which should be of the same
      type, although the code does not enforce this) that should be
      grouped together in user documentation.

      For example, all PDB heuristics could be grouped together so that
      the documention page for the heuristics looks nicer.
    */
    std::unordered_map<std::string, PluginGroupInfo> plugin_group_infos;
    /*
       plugin_infos collects the information about all plugins. This is used,
       for example, to generate the documentation.
    */
    std::unordered_map<std::string, PluginInfo> plugin_infos;
    /*
       Map from predefinition keyword to predefinition function.
    */
    std::unordered_map<std::string, PredefinitionFunction>
        predefinition_functions;

    std::vector<RawPredefinition> predefinitions_raw_ordered;
    std::unordered_map<std::string, std::vector<size_t>>
        predefinitions_raw_by_key;

    void insert_plugin_types(
        const RawRegistry& raw_registry,
        std::vector<std::string>& errors);
    void insert_plugin_groups(
        const RawRegistry& raw_registry,
        std::vector<std::string>& errors);
    void insert_plugins(
        const RawRegistry& raw_registry,
        std::vector<std::string>& errors);

    void insert_plugin(
        const std::string& key,
        const std::any& factory,
        const std::string& group,
        const PluginTypeNameGetter& type_name_factory,
        const std::type_index& type);
    void insert_type_info(const PluginTypeInfo& info);
    void insert_group_info(const PluginGroupInfo& info);

public:
    explicit Registry(const RawRegistry& raw_registry);

    template <typename T>
    std::function<T(OptionParser&)> get_factory(const std::string& key) const
    {
        std::type_index type(typeid(T));
        return std::any_cast<std::function<T(OptionParser&)>>(
            plugin_factories.at(type).at(key));
    }

    bool is_predefinition(const std::string& key) const;
    void handle_predefinition(
        const std::string& key,
        const std::string& arg,
        Predefinitions& predefinitions,
        bool dry_run);
    void handle_repredefinition(
        const std::string& key,
        Predefinitions& predefinitions);
    void handle_all_repredefinition(
        Predefinitions& predefinitions,
        std::unordered_set<std::string> ignore_keys);

    const PluginTypeInfo& get_type_info(const std::type_index& type) const;
    std::vector<PluginTypeInfo> get_sorted_type_infos() const;

    const PluginGroupInfo& get_group_info(const std::string& key) const;

    PluginInfo& get_plugin_info(const std::string& key);

    void add_plugin_info_arg(
        const std::string& key,
        const std::string& arg_name,
        const std::string& help,
        const std::string& type_name,
        const std::string& default_value,
        const Bounds& bounds,
        const ValueExplanations& value_explanations = ValueExplanations());

    void set_plugin_info_synopsis(
        const std::string& key,
        const std::string& name,
        const std::string& description);

    void add_plugin_info_property(
        const std::string& key,
        const std::string& name,
        const std::string& description);

    void add_plugin_info_feature(
        const std::string& key,
        const std::string& feature,
        const std::string& description);

    void add_plugin_info_note(
        const std::string& key,
        const std::string& name,
        const std::string& description,
        bool long_text);

    std::vector<std::string> get_sorted_plugin_info_keys();

    static Registry* instance()
    {
        static Registry instance_(*RawRegistry::instance());
        return &instance_;
    }
};
} // namespace options

#endif
