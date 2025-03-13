#include <iostream>
#include <vector>
#include<algorithm>
#include <string> 
#include <memory>

using namespace std;

class User;
//Book Class
class Book{
    private:
        string title;
        string author;
        string isbn;
        bool avaliable;
    public:
        Book(string t, string a, string i):title(t), author(a),isbn(i),avaliable(true){}
        string returnTitle() const{return title;}
        string returnAuthor() const {return author;}
        string returnISBN() const{return isbn;}
        bool Avaliable() const{return avaliable;}

        void setAvaliable(bool status){avaliable = status;}
};

//Private user data
class User{
    protected: 
        string name;
        int UserID;
        vector<Book*>borrowedBooks;
    public:
        User(string n, int id): name(n), UserID(id){}
        virtual ~User() = default;
        string returnName() const{return name;}
        int returnID() const{return UserID;}

        const vector<Book*>& getBorrowedBooks()const{return borrowedBooks;}
        virtual string returnUsertype() const = 0;

        void borrowBook(Book* book){borrowedBooks.push_back(book);}
        void returnBorrowed(Book* book){
            auto it = find(borrowedBooks.begin(),borrowedBooks.end(),book);
            if (it != borrowedBooks.end()){
                    borrowedBooks.erase(it);
             }
        }
};
    
class Student : public User{
    public:
        Student(string n, int id): User(n, id){}
        string returnUsertype()const override{ return "Student";}
}; 
class Faculty : public User{
    public:
        Faculty(string n, int id): User(n,id){}
        string returnUsertype()const override {return "Faculty";}
};
class Libraryexception : public exception {
    string message;

    public:
        Libraryexception(string msg) : message(msg){}
        const char* what() const noexcept override{return message.c_str();}
};

class Library{
    private:
        static Library* instance;
        vector<unique_ptr<Book>> books;
        vector<unique_ptr<User>> users;
        int nextUserID;

        Library() : nextUserID(0){}
    public:
        static Library& getInstance(){
            if(!instance){
                instance = new Library();
            } return *instance;
        }
        void addBook(string title, string author, string isbn){
            books.push_back(make_unique<Book>(title,author,isbn));
            cout<< ">Book Added\n\n";
        }
        void addUser(int type, string name){
            if (type == 1){
                users.push_back(make_unique<Student>(name,nextUserID++));
            }//user
            else if (type == 2){
                users.push_back(make_unique<Faculty>(name,nextUserID++));
            }//staff
            cout << ">User ADDED with ID "<<nextUserID -1 << "\n\n";
        }

        User* findUser(int id){
            for(auto& user: users){
                if(user-> returnID()== id){
                    return user.get();
                }
            }
            return nullptr;
        }

        Book* findBook(string title){
            for(auto&book :books){
                if(book->returnTitle()==title){
                    return book.get();
                }
            }
            return nullptr;
        } 
        void borrowBook(string title, int userID){
            Book*book = findBook(title);
            User*user = findUser(userID);
            if (!book){
                throw Libraryexception("\n!! Error: No book with that title exists !!\n");
            }
            if (!book->Avaliable()){
                throw Libraryexception("\nUnfortunately this Book is not available.\n");  
            }
            book->setAvaliable(false);
            user->borrowBook(book);
            cout<< "> "<< title<< " has been checked out by User "<< userID <<"\n";
            
            if(!user){
                throw("\n!! Error: No USER with that ID entered exists !!\n");
            }
        }


        void returnBorrowed(string title, int userID){
            Book*book = findBook(title);
            User*user = findUser(userID);
            if(!book){
                throw Libraryexception("!! ERROR: No book with that TITLE exists!!");
            }
            if(!user){
                throw Libraryexception("!! ERROR: No USERNAME with that ID exists!!");
            }
            if(book-> Avaliable()){
                throw Libraryexception("!! ERROR: Book was not borrowed !!");
            }
            user-> returnBorrowed(book);
            book-> setAvaliable(true);
            cout<< "> "<<title<< " has been returned by User "<< userID<<"\n";
        }

        //USER LIST output on menu
        void userList(){
            cout<< ">> List All Users\n";
            cout<< "--------------------";

            for(size_t i = 0; i <users.size(); ++i){
                cout<< ">> Username: " << users[i] -> returnID() << ":\n";
                cout<< ">> Name: "<< users[i]-> returnName()<<"\n";
                cout<< ">> CLASS: "<< users[i]->returnUsertype()<<"\n";
                
                cout<< ">> Books Checked out: \n";
                for(const auto*book : users[i]-> getBorrowedBooks()){
                    cout<< ">> -- - TITLE: \n";
                    cout<< ">> -- -- ---  "<<book ->returnTitle() << "\n";
                    cout<< ">> -- - Author: \n";
                    cout<< ">> -- -- ---  "<< book ->returnAuthor() << "\n";
                    cout<< ">> -- - ISBN #: \n";
                    cout<< ">> -- -- ---   "<< book ->returnISBN() << "\n";
                }
           } 
        }

        //book list output on menu
        void bookList(){
            cout<< ">> List All Books\n";
            cout<< "--------------------";

            for(size_t i = 0; i <books.size(); ++i){
                cout<< ">> Book "<< i << ":\n";
                cout<< ">> Title: "<< books[i]->returnTitle()<<"\n";
                cout<< ">> Author: "<< books[i]->returnAuthor()<<"\n";
                cout<< ">> ISBN #: "<< books[i]->returnISBN()<<"\n";
                cout<< ">> Check Avalibility: "<< (books[i]-> Avaliable()? "Avaliable" : "Unavaliable")<<"\n";

            }
        }  
};
Library* Library::instance = nullptr;
// Choice 1 in Main: Manage Books
void manageBooks(Library& lib){
     while(true){
          try{
            cout<< "> Manage Books: \n";
            cout<< "1. Add a book\n";
            cout<< "2. Edit a book\n";
            cout<< "3. Remove a book\n";
            cout<< "4. Return to previous menu\n";
            cout<< "\nPlease Enter Choice: ";

            int choice;
            cin >> choice;

            if(cin.fail()){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("!! ERROR. Please enter a number from the selection above !!");
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if(choice == 1){
                string title;
                string author;
                string isbn;

                cout<< ">> ADD a book: \n";
                cout<< ">> Please Enter the TITLE or (0 to CANCEL)";
                getline(cin, title);
                if(title == "0")continue;
                
                cout<< ">> Enter the Name of the AUTHOR: \n";
                cout<<"(0 to CANCEL)\n";
                getline(cin,author);
                if(author == "0")continue;

                cout<< ">> Enter the ISBN # associated with the Book: \n";
                cout<<"(0 to CANCEL)\n";
                getline(cin,isbn);
                if(isbn == "0")continue;

                lib.addBook(title,author,isbn);

            } else if(choice ==2) {
                cout<< "!! PERMISSIONS DENIED. Editing the Book Database requires a TEIR 2 (FACULTY) Login. !!";
            } else if(choice == 3){
                while(true){
                    cout<< "\n\n>> Book Management Menu (3)\n";
                    cout<< "1. Is the Book you are interested in Stolen, Damaged or Missing ? \n";
                    cout<< "2. Are you Checking out a book? \n";
                    cout<< "3. Return to the previous menu\n";
                    cout<< "\nPlease enter your choice: ";

                    int choice3;
                    cin >>choice3;

                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        throw runtime_error("!! ERROR. Please enter a number from the selection above !!");
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if(choice3 == 1){
                        cout<< "Your Login Information has been noted and forwarded to the Administration. Please reach out to the Front Desk Librarian IMMIDIEATELY.";
                    }else if(choice3 ==2 ){
                        cout<< "Please visit the (MANAGE TRANSACTIONS) in the MAIN MENU to check out Books.";
                    }else if(choice3 == 3){
                        break;
                    }else{
                        cout<< "!! INVALID INPUT. Please enter a # from the selection above. \n";
                    }
                }         
            }else if (choice ==4){
                break;
            }else{
                cout<<"!! Error, INVALID CHOICE. Please enter a # from the selection\n";
            }

        }catch(const exception& e){
            cout<< "!! ERROR: "<<e.what()<<" !!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
    }
}
//choice 2 in Main: Manage Users
void manageUsers(Library&lib){
    while(true){
    try{
        cout<<"\n> Manage Users:";
        cout<<"1. Add a User ";
        cout<<"2. Edit a User ";
        cout<<"3. Remove a User ";
        cout<<"4. Go Back \n";
        cout<<" Please Enter Your Choice: ";

        int choice2;
        cin >> choice2;

        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            throw runtime_error("!! ERROR Please enter a number from the selection above !!");
        }
        cin.ignore(numeric_limits<streamsize>::max(),'\n');

        if(choice2 == 1){
            while(true){
                    cout<< ">> Add a User:\n";
                    cout<< ">> Enter (1) for Student account or (2) for Faculty\n";
                    cout<< "Press (0) to go back\n";
                    cout<< "Selection: ";
                
                    int num2;
                    cin >> num2;
                
                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        throw runtime_error("!! ERROR Please enter a number from the selection above !!");
                    }
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');

                    if(num2 == 0){
                        break;
                    }
                    if (num2 != 1 && num2 != 2){
                     cout<< "Error. Please select only (1)STUDENT or (2)FACULTY ";
                    }
                    if(num2 == 2){
                        cout<< "Please Enter the 6 digit ADMIN KEY provided in your Job acceptance Email.";
                        cout<<"\nIf you do not have this ADMIN KEY, press (0)";
                        int password;
                        cin >> password;
                        const int adminKey = 654321;

                        if(cin.fail()){
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(),'\n');
                            throw runtime_error("!! ERROR Please enter a number from the selection above !!");
                        }

                        cin.ignore(numeric_limits<streamsize>::max(),'\n');

                        if(adminKey != password){
                            cout<<"The PASSKEY you have provided is incorrect please try again or press (0) to return to menu.";
                        }else if(password == 0){
                            break;
                        }else{
                             cout<< "ACCESS GRANTED\n";
                             cout<< "Your FACULTY new User Application has been sent to your Email. ";
                        }

                    }

                    cout<< "Student Account selected.\n";
                    cout<< ">> Enter username you would like to create\n";
                    cout<< "Press (0) to cancel: ";
                
                    string username;
                    getline(cin,username);

                    if(username == "0"){
                         break;
                     }
                        lib.addUser(num2, username);
                        cout<< ">> Your Student Library Account has been Successfully Created.";
                        break;
                    }
        }else if(choice2 == 2){
             cout<<"!! REQUEST DENIED. Editing Users requires ADMIN Privileges. You Are Currently Using a Teir 1 account !!\n";
        }else if(choice2 == 3){
                cout<<"!! REQUEST DENIED. Deleting Users requires ADMIN Privileges. You Are Currently Using a Teir 1 account !!\n";
        }else if(choice2 == 4){
                break;
        }else{
             cout<<"!! Error, INVALID CHOICE. Please enter a # from the selection\n";
        }
        }catch (const exception& e) {
        cout<< "!! ERROR: "<<e.what()<<" !!\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
   }
}

//Choice 3 in Main Menu: Manage Transactions
void manageTransactions(Library&lib){
    while(true){
        try{
            cout<< "\n> Manage Transactions\n";
            cout<<"1. Check Out a Book\n";
            cout<<"2. Check In a Book\n";
            cout<<"3. List all Books\n";
            cout<<"4. List all Users\n";
            cout<<"5. Go Back\n";
            cout<<"Enter Selection: ";

            int choice;
            cin >> choice;

            if(cin.fail()){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
                throw runtime_error("!! ERROR Please enter a number from the selection above !!");
            }

            cin.ignore(numeric_limits<streamsize>::max(),'\n');

            if(choice == 1){
                string title;
                int userID;

                cout<< ">> Check Out A Book: \n";
                cout<< ">> Book Title (or 0 to cancel): ";
                getline(cin, title);
                if(title == "0") continue;

                while(true){
                    cout<< ">> User ID (or x to cancel):";
                    cin >> userID;

                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        throw runtime_error("!! ERROR Please enter a number from the selection above !!");
                    }
        
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    lib.borrowBook(title,userID);
                    break;
                }
            } else if (choice == 2){
                string title;
                int userID;

                cout<<">> Check In Book: \n";
                cout<<">> Book title: \n";
                cout<<"Enter(0)to cancel";

                getline(cin, title);
                if(title == "0")continue;
                
                while(true){
                    cout<< "User ID (or 0 to cancel):";
                    cin >> userID;

                    if(cin.fail()){
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        cout << "!! ERROR Please enter a numeric user ID !!";
                        continue;
                    }
        
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    lib.returnBorrowed(title,userID);
                    break;
                }
            }else if (choice == 3){
                lib.bookList();
            }else if (choice == 4){
                lib.userList();
            }else if (choice == 5){
                break;
            }else{
                cout << "ERROR. Invalid input. Please enter a # from the selection. \n";
            }

        } catch (const exception& e) {
            cout<< "!! ERROR: "<<e.what()<<" !!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
    }
}
int main(){
    Library&lib = Library:: getInstance();
    while(true){
        try{
        cout << "> Welcome to the Library ";
        cout << "> 1. Manage Books";
        cout << "> 2. Manage Users";
        cout << "> 3. Manage Transactions";
        cout << "> 4. Exit";
        cout << "> Please select from the following: ";
        
        int choice;
        cin >> choice;

        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            throw runtime_error("!! ERROR Please enter a number from the selection above !!");
        }

        cin.ignore(numeric_limits<streamsize>::max(),'\n');

        if(choice == 1){
            manageBooks(lib);
        }else if(choice == 2){
            manageUsers(lib);
        }else if (choice == 3){
            manageTransactions(lib);
        }else if (choice == 4){
            cout << "Thank you for using the Library System Manager. Goodbye.";
            break;
        }
        }catch (const exception& e) {
            cout<< "!! ERROR: "<<e.what()<<" !!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
       }
    } return 0;
}


