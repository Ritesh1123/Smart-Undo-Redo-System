#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*
   Smart Undo/Redo System (Basic C++)
   ----------------------------------
   - Document: stores text
   - Operation (base class) with Insert, Delete, Replace
   - History manager with Undo/Redo using stacks
*/

// ---------------- Document ----------------
class Document {
    string text;
public:
    string getText() { return text; }

    void insertText(int pos, const string &s) {
        if (pos < 0 || pos > (int)text.size()) {
            cout << "Insert: invalid position\n";
            return;
        }
        text.insert(pos, s);
    }

    string deleteText(int pos, int len) {
        if (pos < 0 || pos >= (int)text.size()) {
            cout << "Delete: invalid position\n";
            return "";
        }
        if (pos + len > (int)text.size()) len = text.size() - pos;
        string removed = text.substr(pos, len);
        text.erase(pos, len);
        return removed;
    }

    string replaceText(int pos, int len, const string &s) {
        if (pos < 0 || pos >= (int)text.size()) {
            cout << "Replace: invalid position\n";
            return "";
        }
        if (pos + len > (int)text.size()) len = text.size() - pos;
        string old = text.substr(pos, len);
        text.replace(pos, len, s);
        return old;
    }
};

// ---------------- Operation Base ----------------
class Operation {
public:
    virtual void apply(Document &doc) = 0;
    virtual Operation* invert() = 0;
    virtual ~Operation() {}
};

// ---------------- Insert ----------------
class InsertOp : public Operation {
    int pos;
    string txt;
public:
    InsertOp(int p, string t) { pos = p; txt = t; }
    void apply(Document &doc) { doc.insertText(pos, txt); }
    Operation* invert(); // defined after DeleteOp
};

// ---------------- Delete ----------------
class DeleteOp : public Operation {
    int pos, len;
    string removed;
public:
    DeleteOp(int p, int l, string r="") { pos=p; len=l; removed=r; }
    void apply(Document &doc) {
        removed = doc.deleteText(pos, len);
        len = removed.size();
    }
    Operation* invert() { return new InsertOp(pos, removed); }
};

// now we can safely define InsertOp::invert
Operation* InsertOp::invert() { 
    return new DeleteOp(pos, txt.size(), txt); 
}

// ---------------- Replace ----------------
class ReplaceOp : public Operation {
    int pos, len;
    string newTxt, oldTxt;
public:
    ReplaceOp(int p, int l, string n) { pos=p; len=l; newTxt=n; }
    void apply(Document &doc) { oldTxt = doc.replaceText(pos, len, newTxt); }
    Operation* invert() { return new ReplaceOp(pos, newTxt.size(), oldTxt); }
};

// ---------------- History Manager ----------------
class History {
    vector<Operation*> undoStack;
    vector<Operation*> redoStack;
public:
    Document doc;

    void doOp(Operation *op) {
        op->apply(doc);
        undoStack.push_back(op);
        // clear redo on new op
        for (int i=0;i<(int)redoStack.size();i++) delete redoStack[i];
        redoStack.clear();
    }

    void undo() {
        if (undoStack.empty()) {
            cout << "Nothing to undo\n"; return;
        }
        Operation *op = undoStack.back();
        undoStack.pop_back();
        Operation *inv = op->invert();
        inv->apply(doc);
        redoStack.push_back(op);
    }

    void redo() {
        if (redoStack.empty()) {
            cout << "Nothing to redo\n"; return;
        }
        Operation *op = redoStack.back();
        redoStack.pop_back();
        Operation *inv = op->invert();
        inv->apply(doc);
        undoStack.push_back(op);
    }

    void show() {
        cout << "Document: \"" << doc.getText() << "\"\n";
    }

    ~History() {
        for (int i=0;i<(int)undoStack.size();i++) delete undoStack[i];
        for (int i=0;i<(int)redoStack.size();i++) delete redoStack[i];
    }
};

// ---------------- Main ----------------
int main() {
    History h;
    int choice;
    while (true) {
        cout << "\n1.Insert 2.Delete 3.Replace 4.Undo 5.Redo 6.Show 7.Exit\n";
        cout << "Choice: ";
        cin >> choice;
        if (choice==1) {
            int pos; string s;
            cout << "Pos: "; cin >> pos;
            cout << "Text: "; cin >> s;
            h.doOp(new InsertOp(pos, s));
        }
        else if (choice==2) {
            int pos,len;
            cout << "Pos: "; cin >> pos;
            cout << "Len: "; cin >> len;
            h.doOp(new DeleteOp(pos, len));
        }
        else if (choice==3) {
            int pos,len; string s;
            cout << "Pos: "; cin >> pos;
            cout << "Len: "; cin >> len;
            cout << "New Text: "; cin >> s;
            h.doOp(new ReplaceOp(pos, len, s));
        }
        else if (choice==4) h.undo();
        else if (choice==5) h.redo();
        else if (choice==6) h.show();
        else if (choice==7) break;
        else cout << "Invalid!\n";
    }
    return 0;
}
