#include <runtime/blocks/blockUtils.hpp>
#include "runtime.hpp"
#include <nlohmann/json.hpp>

// TODO: Cleanup duplicate code
// TODO: maybe cache json object if multiple blocks use the same json string?

std::string createArray(const nlohmann::json &json) {
    std::string vals = "[";
    const size_t size = json.size();
    int i = 0;
    for (auto &val : json.items()) {
        vals += val.value().dump();
        if (i < size - 1) vals += ",";
        i++;
    }
    vals += "]";
    return vals;
}

SCRATCH_BLOCK(skyhigh173JSON_json, is_valid) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;

    *outValue = Value(!nlohmann::json::parse(jsonVal.asString(), nullptr, false).is_discarded());

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, is) {
    Value jsonVal;
    Value typeVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "types", thread, sprite, typeVal)) return BlockResult::REPEAT;
    const std::string typeStr = typeVal.asString();

    if (typeStr == "Object") {
        nlohmann::json json;
        json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
        if (json.is_discarded() || !json.is_object()) {
            *outValue = Value(false);
            return BlockResult::CONTINUE;
        }
        *outValue = Value(true);
        return BlockResult::CONTINUE;

    } else if (typeStr == "Array") {
        nlohmann::json json;
        json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
        if (json.is_discarded() || !json.is_array()) {
            *outValue = Value(false);
            return BlockResult::CONTINUE;
        }
        *outValue = Value(true);
        return BlockResult::CONTINUE;
    }

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, get_all) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    const std::string type = Scratch::getFieldValue(*block, "Stype");
    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    if (type == "keys") {

        std::string vals = "[";
        const size_t size = json.size();
        int i = 0;
        for (auto &val : json.items()) {
            vals += '"' + val.key() + '"';
            if (i < size - 1) vals += ",";
            i++;
        }
        vals += "]";
        *outValue = Value(vals);

    } else if (type == "values") {
        *outValue = Value(createArray(json));
    } else if (type == "datas") {

        std::string vals = "[";
        const size_t size = json.size();
        int i = 0;
        for (auto &val : json.items()) {
            vals += '[';
            vals += '"' + val.key() + '"' + ',' + val.value().dump();
            vals += ']';
            if (i < size - 1) vals += ",";
            i++;
        }
        vals += "]";
        *outValue = Value(vals);
    }

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, new) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    const std::string jsonStr = jsonVal.asString();

    if (jsonStr == "Object") {
        *outValue = Value(Value(std::string("{}")));
    } else if (jsonStr == "Array") {
        *outValue = Value(Value(std::string("[]")));
    }

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, has_key) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "key", thread, sprite, keyVal)) return BlockResult::REPEAT;

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded()) {
        *outValue = Value(false);
        return BlockResult::CONTINUE;
    }

    *outValue = Value(json.contains(keyVal.asString()));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, has_value) {
    Value jsonVal;
    Value valVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "value", thread, sprite, valVal)) return BlockResult::REPEAT;
    const std::string valStr = valVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value(false);
        return BlockResult::CONTINUE;
    }

    for (auto &val : json) {
        if (val.is_string() ? (val.get<std::string>() == valStr) : (val.dump() == valStr)) {
            *outValue = Value(true);
            return BlockResult::CONTINUE;
        }
    }

    *outValue = Value(false);
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, equal) {
    Value jsonVal1;
    Value jsonVal2;
    Value equalVal;

    if (!Scratch::getInput(block, "json1", thread, sprite, jsonVal1)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "equal", thread, sprite, equalVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "json2", thread, sprite, jsonVal2)) return BlockResult::REPEAT;

    nlohmann::json json1 = nlohmann::json::parse(jsonVal1.asString(), nullptr, false);
    if (json1.is_discarded()) {
        *outValue = Value(false);
        return BlockResult::CONTINUE;
    }

    nlohmann::json json2 = nlohmann::json::parse(jsonVal2.asString(), nullptr, false);
    if (json2.is_discarded()) {
        *outValue = Value(false);
        return BlockResult::CONTINUE;
    }

    const bool equal = equalVal.asString() == "=";
    // thank goodness nlohmann has an equals operator because I was NOT doing ts myself
    if (equal)
        *outValue = Value(json1 == json2);
    else
        *outValue = Value(json1 != json2);

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, jlength) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded()) {
        *outValue = Value(0);
        return BlockResult::CONTINUE;
    }

    *outValue = Value(int(json.size()));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, get) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const std::string keyStr = keyVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_object() || !json.contains(keyStr)) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    auto val = json.at(keyStr);
    if (val.is_null()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }
    *outValue = Value(val.is_string() ? val.get<std::string>() : val.dump());

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, set) {
    Value jsonVal;
    Value keyVal;
    Value valVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "value", thread, sprite, valVal)) return BlockResult::REPEAT;
    const std::string keyStr = keyVal.asString();
    const std::string valStr = valVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_object()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    json[keyStr] = valStr;
    *outValue = Value(json.dump());

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, delete) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const std::string keyStr = keyVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_object()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    if (json.contains(keyStr)) {
        json.erase(keyStr);
    }

    *outValue = Value(json.dump());

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, length) {
    // it does the exact same thing as the json version
    return block_skyhigh173JSON_json_jlength_(block, thread, sprite, outValue);
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_get) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const double idxNum = keyVal.asDouble();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array() || idxNum < 1 || idxNum > json.size()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }
    auto &j = json[idxNum - 1];
    const std::string value = j.is_string() ? j.get<std::string>() : j.dump();
    *outValue = Value(value);

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_push) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    json.push_back(keyVal.asString());
    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_set) {
    Value jsonVal;
    Value keyVal;
    Value posVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "pos", thread, sprite, posVal)) return BlockResult::REPEAT;
    const int idx = static_cast<int>(posVal.asDouble());

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    if (idx > 0 && idx <= json.size()) {
        json[idx - 1] = keyVal.asString();
    } else if (idx > json.size()) {
        while (json.size() < idx - 1)
            json.push_back(nullptr);
        json[idx - 1] = keyVal.asString();
    }
    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_insert) {
    Value jsonVal;
    Value val;
    Value posVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, val)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "pos", thread, sprite, posVal)) return BlockResult::REPEAT;
    int idx = static_cast<int>(posVal.asDouble());

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    // 0 is the second to last element in the array for some reason?
    if (idx == 0) idx = json.size();

    if (idx > json.size()) {
        json.push_back(val.asString());
    } else {
        size_t insertPos = (idx - 1 < 0) ? 0 : (idx - 1);
        json.insert(json.begin() + insertPos, val.asString());
    }

    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_delete) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const size_t idx = static_cast<size_t>(keyVal.asDouble());

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array() || idx < 1 || idx > json.size()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    json.erase(json.begin() + (static_cast<size_t>(idx) - 1));

    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_remove_all) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const std::string keyStr = keyVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    json.erase(
        std::remove_if(json.begin(), json.end(),
                       [&](const nlohmann::json &el) {
                           return el.is_string() && el.get<std::string>() == keyStr;
                       }),
        json.end());

    *outValue = Value(createArray(json));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_itemH) {
    Value jsonVal;
    Value val;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, val)) return BlockResult::REPEAT;
    const std::string valStr = val.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    for (size_t i = 0; i < json.size(); i++) {
        const auto element = json[i];
        if (element.is_string() && element.get<std::string>() == valStr) {
            *outValue = Value(int(i + 1));
            return BlockResult::CONTINUE;
        }
    }
    *outValue = Value(0);

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_from) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    const std::string jsonStr = jsonVal.asString();

    nlohmann::json json = nlohmann::json::array();

    for (char c : jsonStr) {
        json.push_back(std::string(1, c));
    }

    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_fromto) {
    Value jsonVal;
    Value idxVal1;
    Value idxVal2;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item", thread, sprite, idxVal1)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "item2", thread, sprite, idxVal2)) return BlockResult::REPEAT;
    const int idx1 = static_cast<int>(idxVal1.asDouble());
    const int idx2 = static_cast<int>(idxVal2.asDouble());

    if (idx1 <= 0 || idx2 <= 0 || idx1 >= idx2) {
        *outValue = Value(std::string("[]"));
        return BlockResult::CONTINUE;
    }

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    nlohmann::json array = nlohmann::json::array();

    for (int i = idx1; i <= idx2 && i <= json.size(); i++) {
        array.push_back(json[i - 1]);
    }
    *outValue = Value(createArray(array));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_reverse) {
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    std::reverse(json.begin(), json.end());
    *outValue = Value(createArray(json));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_flat) {
    Value jsonVal;
    Value depthVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "depth", thread, sprite, depthVal)) return BlockResult::REPEAT;
    const int maxDepth = static_cast<int>(depthVal.asDouble());

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    std::function<nlohmann::json(const nlohmann::json &, int)> flatten = [&](const nlohmann::json &arr, int currentDepth) {
        nlohmann::json result = nlohmann::json::array();
        for (const auto &el : arr) {
            if (el.is_array() && currentDepth > 0) {
                nlohmann::json fl = flatten(el, currentDepth - 1);
                result.insert(result.end(), fl.begin(), fl.end());
            } else {
                result.push_back(el);
            }
        }
        return result;
    };

    *outValue = Value(createArray(flatten(json, maxDepth)));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_concat) {
    Value jsonVal1;
    Value jsonVal2;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal1)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "json2", thread, sprite, jsonVal2)) return BlockResult::REPEAT;

    nlohmann::json json1 = nlohmann::json::parse(jsonVal1.asString(), nullptr, false);
    nlohmann::json json2 = nlohmann::json::parse(jsonVal2.asString(), nullptr, false);

    if (json1.is_discarded() || !json1.is_array() || json2.is_discarded() || !json2.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    json1.insert(json1.end(), json2.begin(), json2.end());
    *outValue = Value(createArray(json1));

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_filter) {
    Value jsonVal;
    Value keyVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "key", thread, sprite, keyVal)) return BlockResult::REPEAT;
    const std::string keyStr = keyVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    nlohmann::json result = nlohmann::json::array();
    for (const auto &el : json) {
        if (el.is_object() && el.contains(keyStr)) {
            result.push_back(el[keyStr]);
        }
    }

    *outValue = Value(createArray(result));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_setlen) {
    Value jsonVal;
    Value lenVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "len", thread, sprite, lenVal)) return BlockResult::REPEAT;
    const size_t targetLen = static_cast<size_t>(std::max(0.0, lenVal.asDouble()));

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    if (targetLen < json.size()) {
        json.erase(json.begin() + targetLen, json.end());
    } else {
        while (json.size() < targetLen) {
            json.push_back(nullptr);
        }
    }

    *outValue = Value(createArray(json));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_create) {
    Value textVal;
    Value delimVal;
    if (!Scratch::getInput(block, "text", thread, sprite, textVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "d", thread, sprite, delimVal)) return BlockResult::REPEAT;

    std::string textStr = textVal.asString();
    std::string delimStr = delimVal.asString();
    nlohmann::json result = nlohmann::json::array();

    if (delimStr.empty()) {
        for (char c : textStr) {
            result.push_back(std::string(1, c));
        }
    } else {
        size_t pos = 0;
        while ((pos = textStr.find(delimStr)) != std::string::npos) {
            result.push_back(textStr.substr(0, pos));
            textStr.erase(0, pos + delimStr.length());
        }
        result.push_back(textStr);
    }

    *outValue = Value(createArray(result));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_join) {
    Value jsonVal;
    Value delimVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "d", thread, sprite, delimVal)) return BlockResult::REPEAT;
    const std::string delimStr = delimVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    std::string result = "";
    const size_t size = json.size();
    for (size_t i = 0; i < size; i++) {
        const auto &el = json[i];
        result += el.is_string() ? el.get<std::string>() : el.dump();
        if (i < size - 1) result += delimStr;
    }

    *outValue = Value(result);
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_sort) {
    Value jsonVal;
    Value orderVal;
    if (!Scratch::getInput(block, "list", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "order", thread, sprite, orderVal)) return BlockResult::REPEAT;
    const std::string orderStr = orderVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    std::sort(json.begin(), json.end());

    if (orderStr == "descending" || orderStr == "d") {
        std::reverse(json.begin(), json.end());
    }

    *outValue = Value(createArray(json));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, array_analysis) {
    Value jsonVal;
    Value analysisVal;
    if (!Scratch::getInput(block, "list", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "analysis", thread, sprite, analysisVal)) return BlockResult::REPEAT;
    const std::string type = analysisVal.asString();

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array() || json.empty()) {
        *outValue = Value(0);
        return BlockResult::CONTINUE;
    }

    std::vector<double> numbers;
    for (const auto &el : json) {
        Value v(el.dump());
        numbers.push_back(v.asDouble());
    }

    if (numbers.empty()) {
        *outValue = Value(0);
        return BlockResult::CONTINUE;
    }

    if (type == "minimum") {
        *outValue = Value(*std::min_element(numbers.begin(), numbers.end()));
    } else if (type == "maximum") {
        *outValue = Value(*std::max_element(numbers.begin(), numbers.end()));
    } else if (type == "sum") {
        double sum = 0;
        for (double n : numbers)
            sum += n;
        *outValue = Value(sum);
    } else if (type == "average") {
        double sum = 0;
        for (double n : numbers)
            sum += n;
        *outValue = Value(sum / numbers.size());
    } else if (type == "median") {
        std::sort(numbers.begin(), numbers.end());
        size_t size = numbers.size();
        if (size % 2 == 0) {
            *outValue = Value((numbers[size / 2 - 1] + numbers[size / 2]) / 2.0);
        } else {
            *outValue = Value(numbers[size / 2]);
        }
    } else if (type == "mode") {
        std::map<double, int> counts;
        for (double n : numbers)
            counts[n]++;

        double mode = numbers[0];
        int max = 0;

        for (double n : numbers) {
            if (counts[n] > max) {
                max = counts[n];
                mode = n;
            }
        }
        *outValue = Value(mode);
    } else if (type == "variance") {
        double sum = 0;
        for (double n : numbers)
            sum += n;
        double mean = sum / numbers.size();
        double variance = 0;
        for (double n : numbers)
            variance += (n - mean) * (n - mean);
        *outValue = Value(variance / numbers.size());
    } else {
        *outValue = Value(0);
    }

    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, vm_getlist) {
    Value listVal;
    if (!Scratch::getInput(block, "list", thread, sprite, listVal)) return BlockResult::REPEAT;
    const std::string listId = listVal.asString();

    Sprite *targetSprite = nullptr;
    if (sprite->lists.find(listId) != sprite->lists.end()) targetSprite = sprite;
    if (Scratch::stageSprite->lists.find(listId) != Scratch::stageSprite->lists.end()) targetSprite = Scratch::stageSprite;
    if (!targetSprite) return BlockResult::CONTINUE;

    nlohmann::json array = nlohmann::json::array();

    for (auto &item : targetSprite->lists[listId].items) {
        array.push_back(item.asString());
    }

    *outValue = Value(createArray(array));
    return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(skyhigh173JSON_json, vm_setlist) {
    Value listVal;
    Value jsonVal;
    if (!Scratch::getInput(block, "json", thread, sprite, jsonVal)) return BlockResult::REPEAT;
    if (!Scratch::getInput(block, "list", thread, sprite, listVal)) return BlockResult::REPEAT;
    const std::string listId = listVal.asString();

    Sprite *targetSprite = nullptr;
    if (sprite->lists.find(listId) != sprite->lists.end()) targetSprite = sprite;
    if (Scratch::stageSprite->lists.find(listId) != Scratch::stageSprite->lists.end()) targetSprite = Scratch::stageSprite;
    if (!targetSprite) return BlockResult::CONTINUE;

    nlohmann::json json = nlohmann::json::parse(jsonVal.asString(), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        *outValue = Value("");
        return BlockResult::CONTINUE;
    }

    auto &listItems = targetSprite->lists[listId].items;
    listItems.clear();

    for (auto &val : json.items()) {
        const std::string value = val.value().is_string() ? val.value().get<std::string>() : val.value().dump();
        listItems.push_back(Value(value));
    }

    return BlockResult::CONTINUE;
}

SCRATCH_SHADOW_BLOCK(skyhigh173JSON_menu_types, types);
SCRATCH_SHADOW_BLOCK(skyhigh173JSON_menu_sort_order, sort_order);
SCRATCH_SHADOW_BLOCK(skyhigh173JSON_menu_analysis, analysis);
SCRATCH_SHADOW_BLOCK(skyhigh173JSON_menu_get_list, get_list);
SCRATCH_SHADOW_BLOCK(skyhigh173JSON_menu_equal, equal);