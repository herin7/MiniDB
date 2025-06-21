// A simple, minimalistic DB system built in C++
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <stdexcept>

using namespace std;

struct Column {
    string name;
    string type;
};

vector<Column> schema;

bool loadSchema(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    getline(file, line);
    file.close();

    stringstream ss(line);
    string field;
    while (getline(ss, field, ',')) {
        auto pos = field.find(':');
        if (pos == string::npos) continue;
        schema.push_back({field.substr(0, pos), field.substr(pos + 1)});
    }
    return true;
}

vector<char> serializeRow(const unordered_map<string, string> &data) {
    vector<char> buffer;
    for (const auto &col : schema) {
        if (!data.count(col.name)) throw runtime_error("Missing value for column: " + col.name);
        string val = data.at(col.name);

        if (col.type == "int") {
            try {
                int v = stoi(val);
                buffer.insert(buffer.end(), reinterpret_cast<char*>(&v), reinterpret_cast<char*>(&v) + sizeof(int));
            } catch (...) {
                throw runtime_error("Expected integer for column: " + col.name);
            }
        } else if (col.type == "string" || col.type == "char") {
            int len = val.size();
            buffer.insert(buffer.end(), reinterpret_cast<char*>(&len), reinterpret_cast<char*>(&len) + sizeof(int));
            buffer.insert(buffer.end(), val.begin(), val.end());
        }
    }
    return buffer;
}

unordered_map<string, string> deserializeRow(const vector<char> &buffer) {
    unordered_map<string, string> result;
    size_t offset = 0;
    for (const auto &col : schema) {
        if (col.type == "int") {
            int val;
            memcpy(&val, &buffer[offset], sizeof(int));
            offset += sizeof(int);
            result[col.name] = to_string(val);
        } else if (col.type == "string" || col.type == "char") {
            int len;
            memcpy(&len, &buffer[offset], sizeof(int));
            offset += sizeof(int);
            string str(buffer.begin() + offset, buffer.begin() + offset + len);
            offset += len;
            result[col.name] = str;
        }
    }
    return result;
}

void selectAll() {
    ifstream db("dbfile.db", ios::binary);
    if (!db.is_open()) {
        cerr << "Could not open database file." << endl;
        return;
    }

    // Header
    cout << "+";
    for (const auto &col : schema)
        cout << setw(15) << setfill('-') << "" << "+";
    cout << setfill(' ') << endl;

    cout << "|";
    for (const auto &col : schema)
        cout << setw(15) << col.name << "|";
    cout << endl;

    cout << "+";
    for (const auto &col : schema)
        cout << setw(15) << setfill('-') << "" << "+";
    cout << setfill(' ') << endl;

    while (db.peek() != EOF) {
        int rowSize;
        db.read(reinterpret_cast<char*>(&rowSize), sizeof(int));
        vector<char> row(rowSize);
        db.read(row.data(), rowSize);
        auto values = deserializeRow(row);

        if (!values.count("__deleted__")) {
            cout << "|";
            for (const auto &col : schema)
                cout << setw(15) << values[col.name] << "|";
            cout << endl;
        }
    }
    db.close();
}

void exportCSV(const string &filename) {
    ifstream db("dbfile.db", ios::binary);
    ofstream csv(filename);

    for (size_t i = 0; i < schema.size(); ++i) {
        csv << schema[i].name;
        if (i + 1 < schema.size()) csv << ",";
    }
    csv << "\n";

    while (db.peek() != EOF) {
        int rowSize;
        db.read(reinterpret_cast<char*>(&rowSize), sizeof(int));
        vector<char> row(rowSize);
        db.read(row.data(), rowSize);
        auto values = deserializeRow(row);

        if (!values.count("__deleted__")) {
            for (size_t i = 0; i < schema.size(); ++i) {
                csv << values[schema[i].name];
                if (i + 1 < schema.size()) csv << ",";
            }
            csv << "\n";
        }
    }
    db.close();
    csv.close();
    cout << "Exported data to " << filename << endl;
}

void insertRow(const unordered_map<string, string> &data) {
    auto bytes = serializeRow(data);
    ofstream db("dbfile.db", ios::app | ios::binary);
    int size = bytes.size();
    db.write(reinterpret_cast<char*>(&size), sizeof(int));
    db.write(bytes.data(), size);
    db.close();
}

void updateRow(const string &key, const string &valToMatch, unordered_map<string, string> updates) {
    fstream db("dbfile.db", ios::in | ios::out | ios::binary);
    vector<vector<char>> rows;

    while (db.peek() != EOF) {
        int rowSize;
        db.read(reinterpret_cast<char*>(&rowSize), sizeof(int));
        vector<char> row(rowSize);
        db.read(row.data(), rowSize);
        auto rowMap = deserializeRow(row);

        if (rowMap[key] == valToMatch && !rowMap.count("__deleted__")) {
            for (const auto &entry : updates)
                rowMap[entry.first] = entry.second;
            row = serializeRow(rowMap);
        }
        rows.push_back(row);
    }

    db.close();
    ofstream out("dbfile.db", ios::binary | ios::trunc);
    for (auto &r : rows) {
        int s = r.size();
        out.write(reinterpret_cast<char*>(&s), sizeof(int));
        out.write(r.data(), s);
    }
    out.close();
}

void deleteRow(const string &key, const string &valToMatch) {
    fstream db("dbfile.db", ios::in | ios::out | ios::binary);
    vector<vector<char>> rows;

    while (db.peek() != EOF) {
        int rowSize;
        db.read(reinterpret_cast<char*>(&rowSize), sizeof(int));
        vector<char> row(rowSize);
        db.read(row.data(), rowSize);
        auto rowMap = deserializeRow(row);

        if (rowMap[key] == valToMatch && !rowMap.count("__deleted__")) {
            rowMap["__deleted__"] = "1";
            row = serializeRow(rowMap);
        }
        rows.push_back(row);
    }

    db.close();
    ofstream out("dbfile.db", ios::binary | ios::trunc);
    for (auto &r : rows) {
        int s = r.size();
        out.write(reinterpret_cast<char*>(&s), sizeof(int));
        out.write(r.data(), s);
    }
    out.close();
}

int main(int argc, char* argv[]) {
    if (!loadSchema("schema.txt")) {
        cerr << "Schema file missing or unreadable." << endl;
        return 1;
    }

    string cmd = argc > 1 ? argv[1] : "";

    try {
        if (cmd == "insert") {
            unordered_map<string, string> row;
            for (int i = 2; i < argc; i++) {
                string arg = argv[i];
                size_t pos = arg.find('=');
                if (pos == string::npos) throw runtime_error("Bad insert argument: " + arg);
                row[arg.substr(0, pos)] = arg.substr(pos + 1);
            }
            insertRow(row);
        } else if (cmd == "select") {
            selectAll();
        } else if (cmd == "export") {
            if (argc < 3) throw runtime_error("Filename missing for export");
            exportCSV(argv[2]);
        } else if (cmd == "update") {
            if (argc < 5) throw runtime_error("Usage: update column value key=val...");
            string matchCol = argv[2], matchVal = argv[3];
            unordered_map<string, string> updates;
            for (int i = 4; i < argc; i++) {
                string arg = argv[i];
                size_t pos = arg.find('=');
                if (pos == string::npos) throw runtime_error("Bad update argument: " + arg);
                updates[arg.substr(0, pos)] = arg.substr(pos + 1);
            }
            updateRow(matchCol, matchVal, updates);
        } else if (cmd == "delete") {
            if (argc < 4) throw runtime_error("Usage: delete column value");
            deleteRow(argv[2], argv[3]);
        } else {
            cout << "Available commands:\n";
            cout << "  insert key=val ...\n";
            cout << "  select\n";
            cout << "  update column value key=val ...\n";
            cout << "  delete column value\n";
            cout << "  export filename.csv\n";
        }
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
