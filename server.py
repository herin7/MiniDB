from flask import Flask, request
import subprocess
from flask import Response


app = Flask(__name__, static_folder="static")

def run_db_command(args):
    try:
        result = subprocess.check_output(args)
        return Response(f"<pre>{result.decode()}</pre>", mimetype="text/html")
    except subprocess.CalledProcessError as e:
        return Response(f"<pre>Error: {e.output.decode()}</pre>", mimetype="text/html")

@app.route("/select_all")
def select_all():
    return run_db_command(["mydb.exe", "select"])

@app.route("/insert")
def insert():
    args = ["mydb.exe", "insert"]
    for key in request.args:
        args.append(f"{key}={request.args.get(key)}")
    return run_db_command(args)

@app.route("/update")
def update():
    match_col = request.args.get("match_col")
    match_val = request.args.get("match_val")
    if not match_col or not match_val:
        return "Missing match_col or match_val"
    args = ["mydb.exe", "update", match_col, match_val]
    for key in request.args:
        if key not in ["match_col", "match_val"]:
            args.append(f"{key}={request.args.get(key)}")
    return run_db_command(args)

@app.route("/delete")
def delete():
    col = request.args.get("col")
    val = request.args.get("val")
    if not col or not val:
        return "Missing delete parameters"
    return run_db_command(["mydb.exe", "delete", col, val])

from flask import send_file

@app.route("/export")
def export():
    subprocess.call(["mydb.exe", "export", "data.csv"])
    return send_file("data.csv", as_attachment=True)

@app.route("/")
def index():
    return app.send_static_file("index.html")
from flask import jsonify

@app.route("/schema")
def get_schema():
    try:
        with open("schema.txt") as f:
            line = f.readline().strip()
        fields = []
        for item in line.split(','):
            name, typ = item.split(':')
            fields.append({"name": name, "type": typ})
        return jsonify(fields)  # FIX: ensure it's JSON
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    app.run(debug=True)
