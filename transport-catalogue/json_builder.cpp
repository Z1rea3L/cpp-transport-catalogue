
#include "json_builder.h"
#include <exception>
#include <variant>
#include <utility>

using namespace std::literals;

namespace json {

Builder::Builder()
    : root_()
    , nodes_stack_{&root_}
{}

Node Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Attempt to build JSON which isn't finalized"s);
    }
    return std::move(root_);
}

Builder::DictValueContext Builder::Key(std::string key) {
    Node::Value& host_value = GetCurrentValue();
    
    if (!std::holds_alternative<Dict>(host_value)) {
        throw std::logic_error("Key() outside a dict"s);
    }
    
    nodes_stack_.push_back(
        &std::get<Dict>(host_value)[std::move(key)]
    );
    return BaseContext{*this};
}

Builder::BaseContext Builder::Value(Node::Value value) {
    AddObject(std::move(value), true);
    return *this;
}

Builder::DictItemContext Builder::StartDict() {
    AddObject(Dict{}, false);
    return BaseContext{*this};
}

Builder::ArrayItemContext Builder::StartArray() {
    AddObject(Array{}, false);
    return BaseContext{*this};
}

Builder::BaseContext Builder::EndDict() {
    if (!std::holds_alternative<Dict>(GetCurrentValue())) {
        throw std::logic_error("EndDict() outside a dict"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder::BaseContext Builder::EndArray() {
    if (!std::holds_alternative<Array>(GetCurrentValue())) {
        throw std::logic_error("EndDict() outside an array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Node::Value& Builder::GetCurrentValue() {
    if (nodes_stack_.empty()) {
        throw std::logic_error("Attempt to change finalized JSON"s);
    }
    return nodes_stack_.back()->GetValue();
}

const Node::Value& Builder::GetCurrentValue() const {
    return const_cast<Builder*>(this)->GetCurrentValue();
}

void Builder::AssertNewObjectContext() const {
    if (!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) {
        throw std::logic_error("New object in wrong context"s);
    }
}

void Builder::AddObject(Node::Value value, bool one_shot) {
    Node::Value& host_value = GetCurrentValue();
    if (std::holds_alternative<Array>(host_value)) {
        Node& node
            = std::get<Array>(host_value).emplace_back(std::move(value));
        if (!one_shot) {
            nodes_stack_.push_back(&node);
        }
    } else {
        AssertNewObjectContext();
        host_value = std::move(value);
        if (one_shot) {
            nodes_stack_.pop_back();
        }
    }
}

    Node Builder::BaseContext::Build() {
        return builder_.Build();
    }
    
    Builder::DictValueContext Builder::BaseContext::Key(std::string key) {
        return builder_.Key(std::move(key));
    }
    
    Builder::BaseContext Builder::BaseContext::Value(Node::Value value) {
        return builder_.Value(std::move(value));
    }
    
    Builder::DictItemContext Builder::BaseContext::StartDict() {
        return builder_.StartDict();
    }
    
    Builder::ArrayItemContext Builder::BaseContext::StartArray() {
        return builder_.StartArray();
    }
    
    Builder::BaseContext Builder::BaseContext::EndDict() {
        return builder_.EndDict();
    }
    
    Builder::BaseContext Builder::BaseContext::EndArray() {
        return builder_.EndArray();
    }
    
}  // namespace json
