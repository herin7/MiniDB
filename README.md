# 🔍 MiniDB — Lightweight File-Based Database with Web UI

MiniDB is a compact, fast, and schema-driven database engine built in **C++** with a modern **Flask + HTML** interface. Designed for educational, experimental, and lightweight CRUD applications, MiniDB allows full control over data storage while demonstrating concepts of binary serialization, schema abstraction, and custom persistence without relying on traditional DBMS like SQLite or MySQL.

---

## 🚀 Features

- ✅ Dynamic schema via `schema.txt`
- ✅ C++-based custom binary file storage
- ✅ CRUD support (Insert, Select, Update, Delete)
- ✅ Logical deletes (soft delete)
- ✅ Export records to CSV
- ✅ Web Admin Panel built with Flask
- ✅ Dynamic HTML forms generated based on schema
- ✅ Neatly formatted console + web output

---

## 💼 Why This Project Stands Out

- 🔧 Shows deep understanding of low-level data serialization & file handling in C++
- 🌐 Full-stack integration with Python Flask for admin panel
- 🧠 Demonstrates clean separation between data engine and UI
- 📈 Includes practical features like dynamic forms, CSV export, and update/delete by key

---

## 📦 Folder Structure

```

mydb/
├── mydb.cpp            # C++ database engine
├── schema.txt          # Schema definition file
├── dbfile.db           # Binary data file (auto-generated)
├── app.py              # Flask server for web UI
├── static/
│   └── style.css       # Web UI styles
└── templates/
└── index.html      # Web frontend

```

---

## 🛠️ How to Use

### 1. Define Schema
Edit `schema.txt` to define your table structure:
```

id\:int,name\:string,age\:int,class\:char

````

### 2. Compile the C++ Engine
```bash
g++ -std=c++17 mydb.cpp -o mydb.exe
````

### 3. Run the Web Server

```bash
python app.py
```

Now visit: [http://localhost:5000](http://localhost:5000)

---

## 🔧 Command Line Usage (Optional)

Insert a row:

```bash
./mydb.exe insert id=1 name=Alice age=23 class=A
```

View all:

```bash
./mydb.exe select
```

Update row:

```bash
./mydb.exe update id 1 name=Bob
```

Delete row:

```bash
./mydb.exe delete id 1
```

Export to CSV:

```bash
./mydb.exe export data.csv
```

---

## 🧪 Web Admin Use Cases

* 📋 Insert new rows dynamically based on schema
* 🔍 View all records in real-time
* 🔄 Update any row by matching a key
* ❌ Soft delete rows logically
* 📤 Export entire database to CSV with one click

---

## ✨ Future Enhancements

* Indexing support for faster lookups
* JSON-based REST API
* Authentication layer
* Recycle bin for soft-deleted records
* Backup/restore support

---

## 📚 Technologies Used

* **C++17** — custom file-based database logic
* **Python Flask** — web server
* **HTML/CSS/JS** — dynamic frontend UI
* **Subprocess module** — backend communication

---

## 👨‍💻 Author

**Herin Soni**
B.Tech | Full Stack & Systems Enthusiast
📧 [herin.soni.dev@gmail.com](mailto:herin.soni.dev@gmail.com)
🔗 [GitHub](https://github.com/herin-soni)

---

## 💡 Pro Tip

Want to understand database internals or build something lightweight for embedded/local storage? Start here. This is not just another CRUD app — it’s a mini database engine 🔧

---

## 📎 Demo

> Coming soon — GIFs or screenshots of the web panel in action

