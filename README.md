Library Management System

Overview

The Library Management System is a comprehensive desktop application developed in C++ using Object-Oriented Programming (OOP) principles and the Qt framework. The system is designed to automate and simplify library operations, including resource management, user management, borrowing processes, fine calculation, reporting, and administrative tasks.

This project was developed as a semester project to demonstrate the practical implementation of software engineering concepts, data management, and object-oriented design.

Features

User Management

* User registration and authentication
* Member account management
* Administrative account management
* Secure user validation

Resource Management

* Add, update, search, and remove resources
* Support for physical books and e-books
* Book copy management
* Resource categorization

Borrowing System

* Borrow and return resources
* Borrowing history tracking
* Borrow extension requests
* Availability monitoring

Fine Management

* Automatic fine calculation
* Overdue resource tracking
* Fine payment management

Search and Discovery

* Advanced search functionality
* Resource filtering and lookup
* Book rating system

Notifications and Logging

* User notifications
* Activity logging
* Audit trail management

Reporting and Analytics

* Administrative reports
* Library usage statistics
* Resource analytics
* Performance summaries

Technologies Used

* C++
* Qt Framework
* Object-Oriented Programming (OOP)
* File Handling
* Data Structures
* Exception Handling

OOP Concepts Implemented

* Classes and Objects
* Encapsulation
* Inheritance
* Polymorphism
* Abstraction
* Composition
* Exception Handling

Project Structure

```text
src/
├── Admin
├── User
├── Member
├── Resource
├── Book
├── Ebook
├── LibraryManager
├── BorrowRecord
├── FineManager
├── Notification
├── ReportGenerator
├── AdvancedSearch
├── AdminAnalytics
└── Security Utilities
```

How to Run

Using Qt Creator

1. Open Qt Creator.
2. Open the `LibraryManagementQtApp.pro` file.
3. Configure the project.
4. Build the project.
5. Run the application.

Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Documentation

The repository includes:

* Project Report (`REPORT_CEP.pdf`)
* UML Design (`UML_CEP.mdj`)
* Source Code Documentation

Learning Outcomes

This project demonstrates:

* Real-world application of Object-Oriented Programming
* Software design and architecture principles
* Data management and file handling
* GUI application development using Qt
* Problem-solving and software engineering practices

Authors

Developed as a Computer Engineering Project

License

This project is intended for educational and academic purposes.
