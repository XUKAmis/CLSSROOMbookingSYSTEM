#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <string>
#include <iomanip>
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

    virtual void display() const = 0;

    friend ostream& operator<<(ostream& os, const Classroom& c) {
        os << c.id << " " << c.name << " " << c.capacity << " " << c.occupied << " " << c.type << " " << c.attribute;
        return os;
    }

    friend istream& operator>>(istream& is, Classroom& c) {
        is >> c.id >> c.name >> c.capacity >> c.occupied >> c.type >> c.attribute;
        return is;
    }
};

class Laboratory : public Classroom {
public:
    void display() const override {
        cout << "Laboratory: " << id << " " << name << " " << capacity << " " << occupied << " " << type << " " << attribute << endl;
    }
};

class MultimediaClassroom : public Classroom {
public:
    void display() const override {
        cout << "MultimediaClassroom: " << id << " " << name << " " << capacity << " " << occupied << " " << type << " " << attribute << endl;
    }
};

class RecordingClassroom : public Classroom {
public:
    void display() const override {
        cout << "RecordingClassroom: " << id << " " << name << " " << capacity << " " << occupied << " " << type << " " << attribute << endl;
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

    friend ostream& operator<<(ostream& os, const Teacher& t) {
        os << t.id << " " << t.name << " " << t.level << " " << t.phone << " " << t.office;
        return os;
    }

    friend istream& operator>>(istream& is, Teacher& t) {
        is >> t.id >> t.name >> t.level >> t.phone >> t.office;
        return is;
    }
};

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

    Reservation() : id(next_id++), time(::time(0)), approved(false) {}

    friend ostream& operator<<(ostream& os, const Reservation& r) {
        os << r.id << " " << r.time << " " << r.classroom_id << " " << r.date << " " << r.teacher_id << " " << r.approved;
        return os;
    }

    friend istream& operator>>(istream& is, Reservation& r) {
        is >> r.id >> r.time >> r.classroom_id >> r.date >> r.teacher_id >> r.approved;
        return is;
    }
};

int Reservation::next_id = 1;


void loadClassrooms(vector<Classroom*>& classrooms) {
    ifstream file("classrooms.txt");
    if (file.is_open()) {
        string type;
        while (file >> type) {
            Classroom* classroom = nullptr;
            if (type == "1") classroom = new Laboratory();
            else if (type == "2") classroom = new MultimediaClassroom();
            else if (type == "3") classroom = new RecordingClassroom();
            if (classroom) {
                file >> *classroom;
                classrooms.push_back(classroom);
            }
        }
        file.close();
    }
}

void saveClassrooms(const vector<Classroom*>& classrooms) {
    ofstream file("classrooms.txt");
    for (const auto& classroom : classrooms) {
        file << classroom->type << " " << *classroom << endl;
    }
    file.close();
}

void loadTeachers(vector<Teacher*>& teachers) {
    ifstream file("teachers.txt");
    if (file.is_open()) {
        string type;
        while (file >> type) {
            Teacher* teacher = nullptr;
            if (type == "1") teacher = new Admin();
            else if (type == "2") teacher = new RegularTeacher();
            if (teacher) {
                file >> *teacher;
                teachers.push_back(teacher);
            }
        }
        file.close();
    }
}

void saveTeachers(const vector<Teacher*>& teachers) {
    ofstream file("teachers.txt");
    for (const auto& teacher : teachers) {
        file << teacher->level << " " << *teacher << endl;
    }
    file.close();
}

void loadReservations(vector<Reservation>& reservations) {
    ifstream file("reservations.txt");
    if (file.is_open()) {
        Reservation r;
        while (file >> r) {
            reservations.push_back(r);
        }
        file.close();
    }
}

void saveReservations(const vector<Reservation>& reservations) {
    ofstream file("reservations.txt");
    for (const auto& r : reservations) {
        file << r << endl;
    }
    file.close();
}


void Admin::menu() {
    int choice;
    vector<Classroom*> classrooms;
    vector<Reservation> reservations;
    loadClassrooms(classrooms);
    loadReservations(reservations);

    do {
        cout << "1. View Classrooms\n2. View Reservations\n3. Approve Reservations\n4. Exit\nChoose an option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                for (const auto& classroom : classrooms) {
                    classroom->display();
                }
                break;
            case 2:
                for (const auto& r : reservations) {
                    cout << r.id << " " << ctime(&r.time) << " " << r.classroom_id << " " << r.date << " " << r.teacher_id << " " << (r.approved ? "Approved" : "Pending") << endl;
                }
                break;
            case 3:
                int res_id;
                cout << "Enter reservation ID to approve: ";
                cin >> res_id;
                for (auto& r : reservations) {
                    if (r.id == res_id) {
                        r.approved = true;
                        break;
                    }
                }
                saveReservations(reservations);
                break;
            case 4:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice, try again.\n";
        }
    } while (choice != 4);

    saveClassrooms(classrooms);
    for (auto classroom : classrooms) delete classroom;
}


void RegularTeacher::menu() {
    int choice;
    vector<Classroom*> classrooms;
    vector<Reservation> reservations;
    loadClassrooms(classrooms);
    loadReservations(reservations);

    do {
        cout << "1. View Classrooms\n2. Search Classroom\n3. View My Reservations\n4. Make a Reservation\n5. Cancel a Reservation\n6. Exit\nChoose an option: ";
        cin >> choice;
        switch (choice) {
            case 1:
                for (const auto& classroom : classrooms) {
                    classroom->display();
                }
                break;
            case 2: {
                string name;
                cout << "Enter classroom name to search: ";
                cin >> name;
                for (const auto& classroom : classrooms) {
                    if (classroom->name == name) {
                        classroom->display();
                    }
                }
                break;
            }
            case 3:
                for (const auto& r : reservations) {
                    if (r.teacher_id == this->id) {
                        cout << r.id << " " << ctime(&r.time) << " " << r.classroom_id << " " << r.date << " " << (r.approved ? "Approved" : "Pending") << endl;
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
                    return r.id == res_id && r.teacher_id == this->id;
                });
                if (it != reservations.end()) {
                    reservations.erase(it, reservations.end());
                    saveReservations(reservations);
                    cout << "Reservation canceled.\n";
                } else {
                    cout << "Reservation not found or you do not have permission to cancel it.\n";
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
    for (auto classroom : classrooms) delete classroom;
}

void registerTeacher(vector<Teacher*>& teachers) {
    RegularTeacher* newTeacher = new RegularTeacher();
    cout << "Enter new teacher ID: ";
    cin >> newTeacher->id;
    cout << "Enter new teacher name: ";
    cin >> newTeacher->name;
    cout << "Enter new teacher office: ";
    cin >> newTeacher->office;
    teachers.push_back(newTeacher);
    saveTeachers(teachers);
    cout << "Registration successful. You can now log in with your new ID.\n";
}

int main() {
    vector<Teacher*> teachers;
    loadTeachers(teachers);

    string teacher_id;
    int choice;
    cout << "1. Log in\n2. Register\nChoose an option: ";
    cin >> choice;

    if (choice == 2) {
        registerTeacher(teachers);
    }

    cout << "Enter your ID: ";
    cin >> teacher_id;

    Teacher* logged_in_teacher = nullptr;
    for (auto& teacher : teachers) {
        if (teacher->id == teacher_id) {
            logged_in_teacher = teacher;
            break;
        }
    }

    if (logged_in_teacher) {
        logged_in_teacher->menu();
    } else {
        cout << "Teacher not found.\n";
    }

    saveTeachers(teachers);
    for (auto teacher : teachers) delete teacher;

    return 0;
}