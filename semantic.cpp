#include "semantic.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

namespace {

struct Symbol {
    std::string type;
    int line;
    int column;
};

struct Function {
    std::string returnType;
    std::vector<std::string> parameters;
    int line;
    int column;
};

class Checker {
public:
    Checker(const std::string& filename) : filename_(filename) {
        scopes_.emplace_back();
    }

    bool run(const Node& program) {
        for (const Node& node : program.children) {
            if (node.type == NODE_STRUCT_DEF) {
                if (structs_.count(node.varName))
                    error(node, "повторное объявление структуры '" + node.varName + "'");
                else
                    structs_[node.varName] = &node;
            }
            if (node.type == NODE_FUNC_DEF) {
                Function fn;
                fn.returnType = normalize(node.varType);
                fn.line = node.line;
                fn.column = node.column;
                for (const std::string& param : node.params) {
                    size_t separator = param.find(':');
                    fn.parameters.push_back(normalize(param.substr(0, separator)));
                }
                if (functions_.count(node.varName))
                    error(node, "повторное объявление функции '" + node.varName + "'");
                else
                    functions_[node.varName] = fn;
            }
        }

        for (const Node& node : program.children) {
            if (node.type != NODE_FUNC_DEF && node.type != NODE_STRUCT_DEF)
                checkNode(node);
        }
        for (const Node& node : program.children) {
            if (node.type == NODE_FUNC_DEF)
                checkFunction(node);
            else if (node.type == NODE_STRUCT_DEF)
                checkStruct(node);
        }
        return errors_ == 0;
    }

private:
    std::string filename_;
    unsigned errors_ = 0;
    std::vector<std::map<std::string, Symbol>> scopes_;
    std::map<std::string, Function> functions_;
    std::map<std::string, const Node*> structs_;
    std::map<std::string, std::string> structFields_;
    std::vector<std::string> returnTypes_;
    unsigned loopDepth_ = 0;

    static std::string normalize(const std::string& type) {
        if (type == "int") return "rox";
        if (type == "float") return "dor";
        if (type == "string") return "cos";
        if (type == "bool") return "bue";
        return type;
    }

    static std::string typeName(const std::string& type) {
        if (type == "rox") return "int";
        if (type == "dor") return "float";
        if (type == "cos") return "string";
        if (type == "bue") return "bool";
        if (type.empty()) return "unknown";
        return type;
    }

    void error(const Node& node, const std::string& message) {
        ++errors_;
        std::cerr << filename_ << ':' << node.line << ':' << node.column
                  << ": error[semantic]: " << message << '\n';
    }

    bool knownType(const std::string& raw) const {
        std::string type = normalize(raw);
        return type == "rox" || type == "dor" || type == "cos" ||
               type == "bue" || type == "ptr" || type == "file" ||
               type == "chr" || type == "rox64" || structs_.count(type);
    }

    bool compatible(const std::string& expectedRaw, const std::string& actualRaw) const {
        std::string expected = normalize(expectedRaw);
        std::string actual = normalize(actualRaw);
        return expected == actual || expected == "void" && actual == "<void>" ||
               expected == "dor" && (actual == "rox" || actual == "bue") ||
               expected == "rox64" && (actual == "rox" || actual == "chr");
    }

    bool declare(const std::string& name, const std::string& type,
                 const Node& node) {
        auto& scope = scopes_.back();
        if (scope.count(name)) {
            error(node, "повторное объявление '" + name + "' в этой области видимости");
            return false;
        }
        scope[name] = {normalize(type), node.line, node.column};
        return true;
    }

    const Symbol* lookup(const std::string& name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) return &found->second;
        }
        return nullptr;
    }

    std::string expression(const Node& node) {
        switch (node.type) {
        case NODE_NUMBER:
            if (!node.varType.empty()) return normalize(node.varType);
            return node.value.find('.') == std::string::npos ? "rox" : "dor";
        case NODE_STRING:
            return "cos";
        case NODE_IDENTIFIER: {
            const Symbol* symbol = lookup(node.value);
            if (!symbol) {
                error(node, "неизвестное имя '" + node.value + "'");
                return "<error>";
            }
            return symbol->type;
        }
        case NODE_STRUCT_FIELD: {
            const Symbol* object = lookup(node.varName);
            if (!object) {
                error(node, "неизвестное имя '" + node.varName + "'");
                return "<error>";
            }
            auto structure = structs_.find(object->type);
            if (structure == structs_.end()) {
                error(node, "тип '" + typeName(object->type) + "' не имеет полей");
                return "<error>";
            }
            for (const Node& field : structure->second->children)
                if (field.varName == node.op) return normalize(field.varType);
            error(node, "у структуры '" + object->type + "' нет поля '" + node.op + "'");
            return "<error>";
        }
        case NODE_ARRAY_ACCESS: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) {
                error(node, "неизвестный массив '" + node.varName + "'");
                return "<error>";
            }
            std::string indexType = expression(*node.left);
            if (indexType != "rox" && indexType != "rox64" && indexType != "<error>")
                error(*node.left, "индекс массива должен иметь целочисленный тип");
            if (symbol->type.rfind("array<", 0) == 0)
                return symbol->type.substr(6, symbol->type.size() - 7);
            if (symbol->type == "ptr") return "rox";
            error(node, "значение '" + node.varName + "' не является массивом");
            return "<error>";
        }
        case NODE_CAST: {
            std::string target = normalize(node.varType);
            std::string source = expression(*node.left);
            const std::set<std::string> numeric = {"rox", "dor", "rox64", "chr"};
            if (!knownType(target))
                error(node, "неизвестный тип преобразования '" + node.varType + "'");
            else if (!numeric.count(target) || !numeric.count(source))
                error(node, "явное преобразование между '" + typeName(source) +
                            "' и '" + typeName(target) + "' не поддерживается");
            return target;
        }
        case NODE_ADDR: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) {
                error(node, "невозможно получить адрес неизвестного имени '" + node.varName + "'");
                return "<error>";
            }
            return "ptr";
        }
        case NODE_DEREF: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) {
                error(node, "неизвестный указатель '" + node.varName + "'");
                return "<error>";
            }
            if (symbol->type != "ptr") {
                error(node, "разыменование требует тип ptr, получен '" + typeName(symbol->type) + "'");
                return "<error>";
            }
            return "rox";
        }
        case NODE_DYN_ARRAY: {
            std::string element = normalize(node.varType);
            if (!knownType(element)) error(node, "неизвестный тип элемента массива '" + node.varType + "'");
            std::string sizeType = expression(*node.left);
            if (sizeType != "rox" && sizeType != "rox64" && sizeType != "<error>")
                error(*node.left, "размер массива должен быть целым числом");
            return "array<" + element + ">";
        }
        case NODE_INPUT:
            return node.varType == "cos" ? "cos" : "rox";
        case NODE_FILE_OPEN:
            return "file";
        case NODE_FILE_READ:
            if (!lookup(node.varName)) error(node, "неизвестный файл '" + node.varName + "'");
            return "cos";
        case NODE_BINOP:
            return binary(node);
        case NODE_FUNC_CALL:
            return call(node);
        default:
            error(node, "внутренняя ошибка проверки типов: выражение не поддерживается");
            return "<error>";
        }
    }

    std::string binary(const Node& node) {
        std::string left = expression(*node.left);
        std::string right = expression(*node.right);
        if (left == "<error>" || right == "<error>") return "<error>";
        const std::set<std::string> numeric = {"rox", "dor", "rox64", "chr"};
        if (node.op == ">" || node.op == "<") {
            if (!numeric.count(left) || !numeric.count(right))
                error(node, "оператор '" + node.op + "' принимает только числа");
            return "bue";
        }
        if (node.op == "==" || node.op == "!=") {
            if (!compatible(left, right) && !compatible(right, left))
                error(node, "нельзя сравнивать значения типов '" + typeName(left) +
                            "' и '" + typeName(right) + "'");
            return "bue";
        }
        if (!numeric.count(left) || !numeric.count(right)) {
            error(node, "арифметический оператор '" + node.op + "' требует числовые операнды");
            return "<error>";
        }
        if (left == "dor" || right == "dor") return "dor";
        if (left == "rox64" || right == "rox64") return "rox64";
        return "rox";
    }

    std::string call(const Node& node) {
        static const std::map<std::string, std::pair<size_t, size_t>> builtins = {
            {"sqrt", {1,1}}, {"abs", {1,1}}, {"pow", {2,2}},
            {"max", {2,2}}, {"min", {2,2}}, {"flo", {1,1}},
            {"cel", {1,1}}, {"ron", {1,1}}, {"ran", {0,0}},
            {"ar", {1,1}}, {"ser", {2,2}}, {"rep", {3,3}}, {"str", {1,1}}
        };
        auto builtin = builtins.find(node.varName);
        if (builtin != builtins.end()) {
            if (node.args.size() < builtin->second.first || node.args.size() > builtin->second.second) {
                error(node, "функция '" + node.varName + "' ожидает " +
                      std::to_string(builtin->second.first) + " аргументов, получено " +
                      std::to_string(node.args.size()));
            }
            for (const Node& arg : node.args) {
                std::string type = expression(arg);
                if (type != "rox" && type != "dor" && type != "rox64" && type != "chr" && type != "cos" && type != "<error>")
                    error(arg, "недопустимый тип аргумента функции '" + node.varName + "'");
            }
            if (node.varName == "ser") return "bue";
            if (node.varName == "ar" || node.varName == "ran" || node.varName == "abs" || node.varName == "pow") return "rox";
            if (node.varName == "str" || node.varName == "rep") return "cos";
            if (node.varName == "sqrt" || node.varName == "flo" || node.varName == "cel" || node.varName == "ron") return "rox";
            if (!node.args.empty()) return expression(node.args[0]);
            return "rox";
        }
        auto function = functions_.find(node.varName);
        if (function == functions_.end()) {
            error(node, "неизвестная функция '" + node.varName + "'");
            for (const Node& arg : node.args) expression(arg);
            return "<error>";
        }
        if (node.args.size() != function->second.parameters.size()) {
            error(node, "функция '" + node.varName + "' ожидает " +
                  std::to_string(function->second.parameters.size()) + " аргументов, получено " +
                  std::to_string(node.args.size()));
        }
        size_t count = std::min(node.args.size(), function->second.parameters.size());
        for (size_t i = 0; i < node.args.size(); ++i) {
            std::string actual = expression(node.args[i]);
            if (i < count && actual != "<error>" &&
                !compatible(function->second.parameters[i], actual)) {
                error(node.args[i], "аргумент " + std::to_string(i + 1) + " функции '" +
                      node.varName + "': ожидался '" + typeName(function->second.parameters[i]) +
                      "', получен '" + typeName(actual) + "'");
            }
        }
        return function->second.returnType;
    }

    void checkStruct(const Node& node) {
        std::set<std::string> fields;
        for (const Node& field : node.children) {
            if (!knownType(field.varType)) error(field, "неизвестный тип поля '" + field.varType + "'");
            if (!fields.insert(field.varName).second)
                error(field, "повторное объявление поля '" + field.varName + "'");
            std::string actual = expression(*field.left);
            if (actual != "<error>" && !compatible(field.varType, actual))
                error(field, "значение по умолчанию поля '" + field.varName + "' имеет тип '" +
                      typeName(actual) + "', ожидался '" + typeName(normalize(field.varType)) + "'");
        }
    }

    bool alwaysReturns(const std::vector<Node>& statements) const {
        if (statements.empty()) return false;
        const Node& last = statements.back();
        if (last.type == NODE_RETURN) return true;
        if (last.type == NODE_IF && !last.elseChildren.empty())
            return alwaysReturns(last.children) && alwaysReturns(last.elseChildren);
        return false;
    }

    void checkFunction(const Node& node) {
        scopes_.emplace_back();
        returnTypes_.push_back(normalize(node.varType));
        for (const std::string& param : node.params) {
            size_t separator = param.find(':');
            std::string type = param.substr(0, separator);
            std::string name = separator == std::string::npos ? "" : param.substr(separator + 1);
            if (!knownType(type)) error(node, "неизвестный тип параметра '" + type + "'");
            declare(name, type, node);
        }
        for (const Node& statement : node.children) checkNode(statement);
        if (normalize(node.varType) != "void" && !alwaysReturns(node.children))
            error(node, "функция должна возвращать значение на всех путях выполнения");
        returnTypes_.pop_back();
        scopes_.pop_back();
    }

    void checkNode(const Node& node) {
        switch (node.type) {
        case NODE_VAR_DECL: {
            if (!knownType(node.varType)) error(node, "неизвестный тип переменной '" + node.varType + "'");
            std::string actual = expression(*node.left);
            if (actual != "<error>" && !compatible(node.varType, actual))
                error(node, "переменная '" + node.varName + "' имеет тип '" +
                      typeName(normalize(node.varType)) + "', но инициализируется значением типа '" +
                      typeName(actual) + "'");
            declare(node.varName, node.varType, node);
            break;
        }
        case NODE_ARRAY_DECL: {
            std::string element = normalize(node.varType);
            if (!knownType(element)) error(node, "неизвестный тип элемента массива '" + element + "'");
            for (const Node& value : node.args) {
                std::string actual = expression(value);
                if (actual != "<error>" && !compatible(element, actual))
                    error(value, "элемент массива должен иметь тип '" + typeName(element) + "'");
            }
            declare(node.varName, "array<" + element + ">", node);
            break;
        }
        case NODE_STRUCT_INSTANCE: {
            auto structure = structs_.find(node.varType);
            if (structure == structs_.end()) {
                error(node, "неизвестная структура '" + node.varType + "'");
                break;
            }
            std::map<std::string, const Node*> named;
            std::vector<const Node*> positional;
            for (const Node& init : node.children) {
                if (init.varName.empty()) positional.push_back(&init);
                else if (!named.emplace(init.varName, &init).second)
                    error(init, "поле '" + init.varName + "' инициализировано дважды");
            }
            size_t position = 0;
            for (const Node& field : structure->second->children) {
                const Node* value = nullptr;
                auto it = named.find(field.varName);
                if (it != named.end()) value = it->second;
                else if (position < positional.size()) value = positional[position++];
                if (value) {
                    std::string actual = expression(*value->left);
                    if (actual != "<error>" && !compatible(field.varType, actual))
                        error(*value, "поле '" + field.varName + "' ожидает тип '" +
                              typeName(normalize(field.varType)) + "', получен '" + typeName(actual) + "'");
                }
                if (!value && named.count(field.varName)) named.erase(field.varName);
            }
            for (const auto& item : named) {
                bool exists = false;
                for (const Node& field : structure->second->children)
                    if (field.varName == item.first) exists = true;
                if (!exists) error(*item.second, "у структуры нет поля '" + item.first + "'");
            }
            if (position < positional.size()) error(node, "слишком много позиционных значений структуры");
            declare(node.varName, node.varType, node);
            for (const Node& field : structure->second->children)
                declare(node.varName + "_" + field.varName, field.varType, node);
            break;
        }
        case NODE_ASSIGN: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) {
                auto field = structFields_.find(node.varName);
                if (field == structFields_.end()) {
                    error(node, "присваивание неизвестному имени '" + node.varName + "'");
                    expression(*node.left);
                    break;
                }
            }
            std::string expected = symbol ? symbol->type : structFields_[node.varName];
            std::string actual = expression(*node.left);
            if (actual != "<error>" && !compatible(expected, actual))
                error(node, "нельзя присвоить '" + typeName(actual) + "' значению типа '" + typeName(expected) + "'");
            break;
        }
        case NODE_ARRAY_ASSIGN: {
            const Symbol* array = lookup(node.varName);
            if (!array) error(node, "неизвестный массив '" + node.varName + "'");
            std::string index = expression(*node.left);
            if (index != "rox" && index != "rox64" && index != "<error>") error(*node.left, "индекс массива должен быть целым числом");
            std::string value = expression(*node.right);
            if (array && array->type.rfind("array<", 0) == 0) {
                std::string element = array->type.substr(6, array->type.size() - 7);
                if (value != "<error>" && !compatible(element, value)) error(node, "элемент массива ожидает тип '" + typeName(element) + "'");
            } else if (array && array->type != "ptr") error(node, "значение не является массивом");
            break;
        }
        case NODE_DEREF_ASSIGN: {
            const Symbol* pointer = lookup(node.varName);
            if (!pointer) error(node, "неизвестный указатель '" + node.varName + "'");
            else if (pointer->type != "ptr") error(node, "запись через разыменование требует тип ptr");
            expression(*node.left);
            break;
        }
        case NODE_RETURN: {
            if (returnTypes_.empty()) {
                error(node, "return разрешён только внутри функции");
                expression(*node.left);
                break;
            }
            if (returnTypes_.back() == "void") {
                error(node, "процедура без типа результата не может возвращать значение");
                expression(*node.left);
                break;
            }
            std::string actual = expression(*node.left);
            if (actual != "<error>" && !compatible(returnTypes_.back(), actual))
                error(node, "функция должна вернуть '" + typeName(returnTypes_.back()) +
                      "', получено '" + typeName(actual) + "'");
            break;
        }
        case NODE_FUNC_CALL:
            call(node);
            break;
        case NODE_SLOV:
            if (node.left) expression(*node.left);
            break;
        case NODE_IF: {
            std::string condition = expression(*node.left);
            if (condition != "bue" && condition != "rox" && condition != "<error>")
                error(*node.left, "условие if должно быть bool или int");
            scopes_.emplace_back();
            for (const Node& child : node.children) checkNode(child);
            scopes_.pop_back();
            scopes_.emplace_back();
            for (const Node& child : node.elseChildren) checkNode(child);
            scopes_.pop_back();
            break;
        }
        case NODE_LOOP_N: {
            std::string count = expression(*node.left);
            if (count != "rox" && count != "rox64" && count != "<error>") error(*node.left, "число итераций должно иметь целочисленный тип");
            ++loopDepth_;
            scopes_.emplace_back();
            for (const Node& child : node.children) checkNode(child);
            scopes_.pop_back();
            --loopDepth_;
            break;
        }
        case NODE_LOOP_W: {
            std::string condition = expression(*node.left);
            if (condition != "bue" && condition != "rox" && condition != "<error>") error(*node.left, "условие цикла должно быть bool или int");
            ++loopDepth_;
            scopes_.emplace_back();
            for (const Node& child : node.children) checkNode(child);
            scopes_.pop_back();
            --loopDepth_;
            break;
        }
        case NODE_BREAK:
        case NODE_CONTINUE:
            if (loopDepth_ == 0) error(node, "оператор цикла находится вне цикла");
            break;
        case NODE_STRUCT_DEF:
        case NODE_FUNC_DEF:
        case NODE_IMPORT:
            break;
        case NODE_DELETE: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) error(node, "неизвестное имя '" + node.varName + "'");
            else if (symbol->type.rfind("array<", 0) != 0 && symbol->type != "ptr") error(node, "освободить можно только динамический массив или ptr");
            break;
        }
        case NODE_FILE_WRITE:
        case NODE_FILE_CLOSE: {
            const Symbol* symbol = lookup(node.varName);
            if (!symbol) error(node, "неизвестный файл '" + node.varName + "'");
            else if (symbol->type != "file") error(node, "ожидался file, получен '" + typeName(symbol->type) + "'");
            if (node.left) {
                std::string value = expression(*node.left);
                if (node.type == NODE_FILE_WRITE && value != "cos" && value != "<error>") error(node, "запись в файл принимает string");
            }
            break;
        }
        default:
            expression(node);
            break;
        }
    }
};

} // namespace

bool checkSemantics(const Node& program, const std::string& filename) {
    Checker checker(filename);
    return checker.run(program);
}
