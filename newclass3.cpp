#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;

class Classroom {
public:
    string id;
    string name;
    int capacity;
    bool occupied;
    int type;
    int attribute;

    virtual void display() = 0;
    friend ostream& operator<<(ostream& os, const Classroom& c);
    friend istream& operator>>(istream& is, Classroom& c);
};

ostream& operator<<(ostream& os, const Classroom& c) {
    os << c.id << " " << c.name << " " << c.capacity << " " << c.occupied << " " << c.type << " " << c.attribute;
    return os;
}

istream& operator>>(istream& is, Classroom& c) {
    is >> c.id >> c.name >> c.capacity >> c.occupied >> c.type >> c.attribute;
    return is;
}

class Lab : public Classroom {
public:
    void display() override {
        cout << "Lab: " << id << " " << name << " " << capacity << " " << occupied << " " << attribute << endl;
    }
};

class MultimediaRoom : public Classroom {
public:
    void display() override {
        cout << "MultimediaRoom: " << id << " " << name << " " << capacity << " " << occupied << " " << attribute << endl;
    }
};

class RecordingStudio : public Classroom {
public:
    void display() override {
        cout << "RecordingStudio: " << id << " " << name << " " << capacity << " " << occupied << " " << attribute << endl;
    }
};

class Teacher {
public:
    string id;
    string name;
    int level;
    string phone;
    string office;

    virtual void menu() = 0;
    friend ostream& operator<<(ostream& os, const Teacher& t);
    friend istream& operator>>(istream& is, Teacher& t);
};

ostream& operator<<(ostream& os, const Teacher& t) {
    os << t.id << " " << t.name << " " << t.level << " " << t.phone << " " << t.office;
    return os;
}

istream& operator>>(istream& is, Teacher& t) {
    is >> t.id >> t.name >> t.level >> t.phone >> t.office;
    return is;
}

class Admin : public Teacher {
public:
    void menu() override;
};

class RegularTeacher : public Teacher {
public:
    void menu() override;
};

class Reservation {
public:
    static int next_id;
    int id;
    time_t time;
    string classroom_id;
    string date;
    string teacher_id;
    bool approved;

    Reservation() : id(next_id++), approved(false) {
        time = std::time(nullptr);
    }

    friend ostream& operator<<(ostream& os, const Reservation& r);
    friend istream& operator>>(istream& is, Reservation& r);
};

int Reservation::next_id = 1;

ostream& operator<<(ostream& os, const Reservation& r) {
    os << r.id << " " << r.time << " " << r.classroom_id << " " << r.date << " " << r.teacher_id << " " << r.approved;
    return os;
}

istream& operator>>(istream& is, Reservation& r) {
    is >> r.id >> r.time >> r.classroom_id >> r.date >> r.teacher_id >> r.approved;
    return is;
}


void loadClassrooms(vector<Classroom*>& classrooms) {
    ifstream file("classrooms.txt");
    if (!file) return;

    while (!file.eof()) {
        Classroom* c;
        int type;
        file >> type;
        switch (type) {
            case 1: c = new Lab(); break;
            case 2: c = new MultimediaRoom(); break;
            case 3: c = new RecordingStudio(); break;
            default: continue;
        }
        file >> *c;
        classrooms.push_back(c);
    }
    file.close();
}

void saveClassrooms(const vector<Classroom*>& classrooms) {
    ofstream file("classrooms.txt");
    for (const auto& c : classrooms) {
        file << c->type << " " << *c << endl;
    }
    file.close();
}

void loadTeachers(vector<Teacher*>& teachers) {
    ifstream file("teachers.txt");
    if (!file) return;

    while (!file.eof()) {
        Teacher* t;
        int level;
        file >> level;
        switch (level) {
            case 1: t = new Admin(); break;
            case 2: t = new RegularTeacher(); break;
            default: continue;
        }
        file >> *t;
        teachers.push_back(t);
    }
    file.close();
}

void saveTeachers(const vector<Teacher*>& teachers) {
    ofstream file("teachers.txt");
    for (const auto& t : teachers) {
        file << t->level << " " << *t << endl;
    }
    file.close();
}

void loadReservations(vector<Reservation>& reservations) {
    ifstream file("reservations.txt");
    if (!file) return;

    Reservation r;
    while (file >> r) {
        reservations.push_back(r);
    }
    file.close();
}

void saveReservations(const vector<Reservation>& reservations) {
    ofstream file("reservations.txt");
    for (const auto& r : reservations) {
        file << r << endl;
    }
    file.close();
}

void Admin::menu() {
    vector<Classroom*> classrooms;
    vector<Reservation> reservations;
    loadClassrooms(classrooms);
    loadReservations(reservations);

    int choice;
    do {
        cout << "Admin Menu:\n1. View Classrooms\n2. View Reservations\n3. Approve Reservation\n4. Exit\nChoose an option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                for (const auto& c : classrooms) {
                    c->display();
                }
                break;
            case 2:
                for (const auto& r : reservations) {
                    cout << r << endl;
                }
                break;
            case 3: {
                int res_id;
                cout << "Enter reservation ID to approve: ";
                cin >> res_id;
                for (auto& r : reservations) {
                    if (r.id == res_id) {
                        r.approved = true;
                        cout << "Reservation approved.\n";
                        break;
                    }
                }
                saveReservations(reservations);
                break;
            }
            case 4:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice, try again.\n";
        }
    } while (choice != 4);

    saveClassrooms(classrooms);
    for (auto c : classrooms) delete c;
}

void RegularTeacher::menu() {
    vector<Classroom*> classrooms;
    vector<Reservation> reservations;
    loadClassrooms(classrooms);
    loadReservations(reservations);

    int choice;
    do {
        cout << "Teacher Menu:\n1. View Classrooms\n2. Search Classroom\n3. View My Reservations\n4. Make a Reservation\n5. Cancel a Reservation\n6. Exit\nChoose an option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                for (const auto& c : classrooms) {
                    c->display();
                }
                break;
            case 2: {
                string name;
                cout << "Enter classroom name to search: ";
                cin >> name;
                for (const auto& c : classrooms) {
                    if (c->name == name) {
                        c->display();
                    }
                }
                break;
            }
            case 3:
                for (const auto& r : reservations) {
                    if (r.teacher_id == this->id) {
                        cout << r << endl;
                    }
                }
                break;
            case 4: {
                Reservation r;
                r.teacher_id = this->id;
                cout << "Enter classroom ID to reserve: ";
                cin >> r.classroom_id;
                cout << "Enter date (YYYY-MM-DD): ";
                cin >> r.date;
                reservations.push_back(r);
                saveReservations(reservations);
                break;
            }
            case 5: {
                int res_id;
                cout << "Enter reservation ID to cancel: ";
                cin >> res_id;
                auto it = remove_if(reservations.begin(), reservations.end(), [&](Reservation& r) {
                    return r.id == res_id && r.teacher_id == this->id && !r.approved;
                });
                if (it != reservations.end()) {
                    reservations.erase(it, reservations.end());
                    saveReservations(reservations);
                    cout << "Reservation canceled.\n";
                } else {
                    cout << "Reservation not found or cannot be canceled.\n";
                }
                break;
            }
            case 6:
    cout << "Exiting...\n";
    break;
default:
    cout << "Invalid choice, try again.\n";
}
} while (choice != 6);

saveClassrooms(classrooms);
for (auto c : classrooms) delete c;
}

void mainMenu() {
vector<Teacher*> teachers;
loadTeachers(teachers);

int choice;
string id, password;

do {
    cout << "Main Menu:\n1. Login\n2. Register as Teacher\n3. Exit\nChoose an option: ";
    cin >> choice;
    switch (choice) {
        case 1: {
            cout << "Enter ID: ";
            cin >> id;
            cout << "Enter Password: ";
            cin >> password;
            bool found = false;
            for (auto t : teachers) {
                if (t->id == id) {
                    found = true;
                    t->menu();
                    break;
                }
            }
            if (!found) {
                cout << "Teacher not found.\n";
            }
            break;
        }
        case 2: {
            RegularTeacher* newTeacher = new RegularTeacher();
            cout << "Enter ID: ";
            cin >> newTeacher->id;
            cout << "Enter Name: ";
            cin >> newTeacher->name;
            cout << "Enter Phone: ";
            cin >> newTeacher->phone;
            cout << "Enter Office: ";
            cin >> newTeacher->office;
            newTeacher->level = 2;
            teachers.push_back(newTeacher);
            saveTeachers(teachers);
            break;
        }
        case 3:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice, try again.\n";
    }
} while (choice != 3);

for (auto t : teachers) delete t;
}

int main() {
mainMenu();
return 0;
}