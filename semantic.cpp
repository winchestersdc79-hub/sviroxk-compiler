#include "semantic.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <limits>
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
        if (type == "int" || type == "i32" || type == "rox") return "i32";
        if (type == "uint" || type == "u32") return "u32";
        if (type == "short" || type == "i16") return "i16";
        if (type == "ushort" || type == "u16") return "u16";
        if (type == "long" || type == "i64" || type == "rox64") return "i64";
        if (type == "ulong" || type == "u64") return "u64";
        if (type == "byte" || type == "u8") return "u8";
        if (type == "float" || type == "f32" || type == "dor") return "f32";
        if (type == "double" || type == "f64") return "f64";
        if (type == "string" || type == "str" || type == "cos") return "str";
        if (type == "bool" || type == "bue") return "bool";
        if (type == "chr") return "char";
        return type;
    }

    static std::string typeName(const std::string& type) {
        if (type == "i32") return "int/i32";
        if (type == "u32") return "uint/u32";
        if (type == "i16") return "short/i16";
        if (type == "u16") return "ushort/u16";
        if (type == "i64") return "long/i64";
        if (type == "u64") return "ulong/u64";
        if (type == "u8") return "byte/u8";
        if (type == "f32") return "float/f32";
        if (type == "f64") return "double/f64";
        if (type == "str") return "str";
        if (type.empty()) return "unknown";
        return type;
    }

    static bool literalFits(const std::string& type, const std::string& text) {
        try {
            if (type == "f32" || type == "f64") {
                size_t parsed = 0;
                double value = std::stod(text, &parsed);
                if (parsed != text.size() || !std::isfinite(value)) return false;
                return type != "f32" || std::fabs(value) <= std::numeric_limits<float>::max();
            }
            if (type == "i8") { size_t n = 0; auto v = std::stoll(text, &n); return n == text.size() && v >= -128 && v <= 127; }
            if (type == "i16") { size_t n = 0; auto v = std::stoll(text, &n); return n == text.size() && v >= -32768 && v <= 32767; }
            if (type == "i32") { size_t n = 0; auto v = std::stoll(text, &n); return n == text.size() && v >= INT32_MIN && v <= INT32_MAX; }
            if (type == "i64") { size_t n = 0; std::stoll(text, &n); return n == text.size(); }
            if (type == "u8") { size_t n = 0; auto v = std::stoull(text, &n); return n == text.size() && text[0] != '-' && v <= 255; }
            if (type == "u16") { size_t n = 0; auto v = std::stoull(text, &n); return n == text.size() && text[0] != '-' && v <= 65535; }
            if (type == "u32") { size_t n = 0; auto v = std::stoull(text, &n); return n == text.size() && text[0] != '-' && v <= UINT32_MAX; }
            if (type == "u64") { size_t n = 0; std::stoull(text, &n); return n == text.size() && !text.empty() && text[0] != '-'; }
        } catch (...) { return false; }
        return false;
    }

    void error(const Node& node, const std::string& message) {
        ++errors_;
        std::cerr << filename_ << ':' << node.line << ':' << node.column
                  << ": error[semantic]: " << message << '\n';
    }

    bool knownType(const std::string& raw) const {
        std::string type = normalize(raw);
        return type == "i8" || type == "i16" || type == "i32" || type == "i64" ||
               type == "u8" || type == "u16" || type == "u32" || type == "u64" ||
               type == "f32" || type == "f64" || type == "str" || type == "bool" ||
               type == "char" || type == "ptr" || type == "file" || structs_.count(type);
    }

    static bool isInteger(const std::string& type) {
        return type == "i8" || type == "i16" || type == "i32" || type == "i64" ||
               type == "u8" || type == "u16" || type == "u32" || type == "u64";
    }

    bool compatible(const std::string& expectedRaw, const std::string& actualRaw) const {
        std::string expected = normalize(expectedRaw);
        std::string actual = normalize(actualRaw);
        if (expected == actual ||
            ((expected == "f32" || expected == "f64") &&
             (actual == "i8" || actual == "i16" || actual == "i32" || actual == "i64" ||
              actual == "u8" || actual == "u16" || actual == "u32" || actual == "u64")) ||
            (expected == "i64" && (actual == "i8" || actual == "i16" || actual == "i32")) ||
            (expected == "u64" && (actual == "u8" || actual == "u16" || actual == "u32"))) return true;
        return false;
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
        case NODE_NUMBER: {
            const std::string literalType = !node.varType.empty()
                ? normalize(node.varType)
                : node.value.find_first_of(".eE") == std::string::npos ? "i32" : "f64";
            if ((literalType == "f32" || literalType == "f64" || isInteger(literalType)) &&
                !literalFits(literalType, node.value)) {
                error(node, "числовой литерал вне диапазона типа '" + typeName(literalType) + "'");
                return "<error>";
            }
            return literalType;
        }
        case NODE_STRING:
            return "str";
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
            if (!isInteger(indexType) && indexType != "<error>")
                error(*node.left, "индекс массива должен иметь целочисленный тип");
            if (symbol->type.rfind("array<", 0) == 0)
                return symbol->type.substr(6, symbol->type.size() - 7);
            if (symbol->type == "ptr") return "i32";
            error(node, "значение '" + node.varName + "' не является массивом");
            return "<error>";
        }
        case NODE_CAST: {
            std::string target = normalize(node.varType);
            std::string source = expression(*node.left);
            const std::set<std::string> numeric = {"i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64"};
            if (!knownType(target))
                error(node, "неизвестный тип преобразования '" + node.varType + "'");
            else if (!numeric.count(target) || !numeric.count(source))
                error(node, "явное преобразование между '" + typeName(source) +
                            "' и '" + typeName(target) + "' не поддерживается");
            else if (node.left->type == NODE_NUMBER &&
                     isInteger(target) && !literalFits(target, node.left->value))
                error(node, "числовой литерал вне диапазона типа '" + typeName(target) + "'");
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
            return "i32";
        }
        case NODE_DYN_ARRAY: {
            std::string element = normalize(node.varType);
            if (!knownType(element)) error(node, "неизвестный тип элемента массива '" + node.varType + "'");
            std::string sizeType = expression(*node.left);
            if (!isInteger(sizeType) && sizeType != "<error>")
                error(*node.left, "размер массива должен быть целым числом");
            return "array<" + element + ">";
        }
        case NODE_INPUT:
            return node.varType == "cos" ? "str" : "i32";
        case NODE_FILE_OPEN:
            return "file";
        case NODE_FILE_READ:
            if (!lookup(node.varName)) error(node, "неизвестный файл '" + node.varName + "'");
            return "str";
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
        if (node.op == "!") {
            if (left == "<error>") return "<error>";
            if (left != "bool" && !isInteger(left))
                error(node, "оператор ! ожидает bool или целое число");
            return "bool";
        }
        std::string right = node.right ? expression(*node.right) : std::string("<error>");
        if (left == "<error>" || right == "<error>") return "<error>";
        if (node.op == "&&" || node.op == "||") {
            if ((left != "bool" && !isInteger(left)) ||
                (right != "bool" && !isInteger(right)))
                error(node, "логические операторы ожидают bool или целые числа");
            return "bool";
        }
        if (isInteger(left) && isInteger(right) &&
            (node.op == ">" || node.op == "<" || node.op == ">=" || node.op == "<=" ||
             node.op == "==" || node.op == "!=")) {
            const bool leftUnsigned = left[0] == 'u';
            const bool rightUnsigned = right[0] == 'u';
            if (leftUnsigned != rightUnsigned) {
                error(node, "нельзя сравнивать signed- и unsigned-типы без явного преобразования");
                return "<error>";
            }
        }
        if (isInteger(left) && isInteger(right) &&
            node.op != ">" && node.op != "<" && node.op != ">=" && node.op != "<=" &&
            node.op != "==" && node.op != "!=") {
            const bool leftUnsigned = left[0] == 'u';
            const bool rightUnsigned = right[0] == 'u';
            if (leftUnsigned != rightUnsigned) {
                const std::string unsignedType = leftUnsigned ? left : right;
                const std::string signedType = leftUnsigned ? right : left;
                const unsigned unsignedBits = static_cast<unsigned>(std::stoi(unsignedType.substr(1)));
                const unsigned signedBits = static_cast<unsigned>(std::stoi(signedType.substr(1)));
                if (std::max(unsignedBits, signedBits) < 32) return "i32";
                if (signedBits > unsignedBits) return signedType;
                error(node, "нельзя смешивать signed- и unsigned-типы этой ширины без явного преобразования");
                return "<error>";
            }
        }
        const std::set<std::string> numeric = {"i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64"};
        const bool comparison = node.op == ">" || node.op == "<" || node.op == ">=" ||
            node.op == "<=" || node.op == "==" || node.op == "!=";
        if (comparison) {
            if (!numeric.count(left) || !numeric.count(right)) {
                error(node, "оператор сравнения принимает только числа");
                return "<error>";
            }
            if (isInteger(left) && isInteger(right) && left[0] != right[0]) {
                error(node, "нельзя сравнивать signed- и unsigned-типы без явного преобразования");
                return "<error>";
            }
            return "bool";
        }
        if (!numeric.count(left) || !numeric.count(right)) {
            error(node, "арифметический оператор '" + node.op + "' требует числовые операнды");
            return "<error>";
        }
        if (left == "f64" || right == "f64") return "f64";
        if (left == "f32" || right == "f32") return "f32";
        if (left == "u64" || right == "u64") return "u64";
        if (left == "i64" || right == "i64") return "i64";
        if (left == "u32" || right == "u32") return "u32";
        if (left == "i32" || right == "i32") return "i32";
        if (left[0] == 'u') return "u32";
        return "i32";
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
                if (!isInteger(type) && type != "f32" && type != "f64" && type != "char" && type != "str" && type != "<error>")
                    error(arg, "недопустимый тип аргумента функции '" + node.varName + "'");
            }
            if (node.varName == "ser") return "bool";
            if (node.varName == "ar" || node.varName == "ran" || node.varName == "abs" || node.varName == "pow") return "i32";
            if (node.varName == "str" || node.varName == "rep") return "str";
            if (node.varName == "sqrt" || node.varName == "flo" || node.varName == "cel" || node.varName == "ron") return "i32";
            if (!node.args.empty()) return expression(node.args[0]);
            return "i32";
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
            const std::string expectedType = i < count
                ? normalize(function->second.parameters[i]) : std::string();
            const bool fittingLiteral = i < count && node.args[i].type == NODE_NUMBER &&
                literalFits(expectedType, node.args[i].value);
            if (i < count && actual != "<error>" &&
                !compatible(function->second.parameters[i], actual) && !fittingLiteral) {
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
            if (actual != "<error>" && !compatible(field.varType, actual)) {
                bool fittingLiteral = field.left->type == NODE_NUMBER &&
                    literalFits(normalize(field.varType), field.left->value);
                if (!fittingLiteral)
                    error(field, "значение по умолчанию поля '" + field.varName + "' имеет тип '" +
                          typeName(actual) + "', ожидался '" + typeName(normalize(field.varType)) + "'");
            }
            if (field.left->type == NODE_NUMBER &&
                !literalFits(normalize(field.varType), field.left->value))
                error(field, "литерал поля вне диапазона типа '" + typeName(normalize(field.varType)) + "'");
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
            const std::string declaredType = normalize(node.varType);
            const bool numericLiteral = node.left->type == NODE_NUMBER &&
                (isInteger(declaredType) || declaredType == "f32" || declaredType == "f64");
            const bool literalInRange = numericLiteral && literalFits(declaredType, node.left->value);
            if (numericLiteral && !literalInRange) {
                error(node, "числовой литерал вне диапазона типа '" + typeName(declaredType) + "'");
            } else if (actual != "<error>" && !compatible(node.varType, actual) && !literalInRange) {
                error(node, "переменная '" + node.varName + "' имеет тип '" +
                      typeName(declaredType) + "', но инициализируется значением типа '" +
                      typeName(actual) + "'");
            }
            declare(node.varName, node.varType, node);
            break;
        }
        case NODE_ARRAY_DECL: {
            std::string element = normalize(node.varType);
            if (element != "i32" && element != "f64" && element != "str" && element != "bool")
                error(node, "массивы точных типов пока не поддерживаются backend'ом; используйте i32, f64, str или bool");
            if (!knownType(element)) error(node, "неизвестный тип элемента массива '" + element + "'");
            for (const Node& value : node.args) {
                std::string actual = expression(value);
                if (actual != "<error>" && !compatible(element, actual))
                    error(value, "элемент массива должен иметь тип '" + typeName(element) + "'");
            }
            if (element != "i32" && element != "f64" && element != "str" && element != "bool")
                error(node, "backend пока не поддерживает массивы типа '" + typeName(element) + "'");
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
                    if (value->left->type == NODE_NUMBER &&
                        !literalFits(normalize(field.varType), value->left->value))
                        error(*value, "литерал поля вне диапазона типа '" + typeName(normalize(field.varType)) + "'");
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
            if (!isInteger(index) && index != "<error>") error(*node.left, "индекс массива должен быть целым числом");
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
            bool fittingLiteral = node.left->type == NODE_NUMBER &&
                literalFits(returnTypes_.back(), node.left->value);
            if (node.left->type == NODE_NUMBER && !literalFits(returnTypes_.back(), node.left->value))
                error(node, "числовой литерал возврата вне диапазона типа '" + typeName(returnTypes_.back()) + "'");
            else if (actual != "<error>" && !compatible(returnTypes_.back(), actual) && !fittingLiteral)
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
            if (condition != "bool" && !isInteger(condition) && condition != "<error>")
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
            if (!isInteger(count) && count != "<error>") error(*node.left, "число итераций должно иметь целочисленный тип");
            ++loopDepth_;
            scopes_.emplace_back();
            for (const Node& child : node.children) checkNode(child);
            scopes_.pop_back();
            --loopDepth_;
            break;
        }
        case NODE_LOOP_W: {
            std::string condition = expression(*node.left);
            if (condition != "bool" && !isInteger(condition) && condition != "<error>") error(*node.left, "условие цикла должно быть bool или int");
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
