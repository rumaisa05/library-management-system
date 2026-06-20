===============================================================================
               LIBRARY AND RESOURCE MANAGEMENT SYSTEM
                    C++ OOP CEP Project  |  CS-116
===============================================================================

  Developed By  :  Rumaisa Waseem  (CS-25052)
                   Laiba Qasim     (CS-25069)
                   Aleena Khalid   (CS-25056)

  Section       :  B
  Semester      :  Spring 2026
  Department    :  Computer & Information Systems Engineering
  University    :  NED University of Engineering & Technology

===============================================================================


===============================================================================
  1. PROJECT OVERVIEW
===============================================================================

The Library Management System is a console-based application developed in
C++ using advanced Object-Oriented Programming concepts and modular software
design principles.

It simulates a real-world library environment where members can borrow,
return, and reserve books, pay fines, and administrators can manage
resources, users, analytics, and reports efficiently.

Core Objectives:
  - Implement advanced OOP concepts in C++ (inheritance, polymorphism,
    encapsulation, abstraction)
  - Simulate a complete real-world library management environment
  - Design a modular, maintainable, and scalable software architecture
  - Automate traditional library operations with secure authentication
    and role-based access control
  - Store all records permanently using file handling across sessions
  - Generate reports and analytics for administrators


===============================================================================
  2. HOW TO RUN THE PROJECT
===============================================================================

PREREQUISITES
-------------------------------------------------------------------------------
You need one of the following C++ compilers:
  - g++ (MinGW) -- recommended for Windows PowerShell
  - Visual Studio (MSVC)

You also need:
  - Visual Studio Code (or any C++ IDE)
  - Windows terminal / PowerShell


STEP 01 -- FIRST-RUN WORKSPACE SETUP
-------------------------------------------------------------------------------
The project uses persistent data files (users.txt, resources.txt,
borrow_records.txt, etc.). Before compiling for the first time you must
create a fresh runnable workspace on your Desktop.

  Step 1.0 -- Extract the ZIP on Desktop
    1. Locate the downloaded project .zip file.
    2. Right-click the .zip file -> Extract All...
    3. Set the destination to Desktop (recommended).

  Step 1.1 -- A Folder named "G2-08_CEP" will be created
    1. Go to Desktop.
    2. Check for folder named:   G2-08_CEP

  Step 1.3 -- Verify Files 
    Open Desktop\G2-08_CEP and confirm it contains
    Total 47 items - which includes ( 22 .h files and 22 .cpp files )
========================================================================================
    NOTE : These 47 items includes UML Diagram , Report , README.txt and program files
========================================================================================	

STEP 2 -- OPEN PROJECT FOLDER IN VS CODE
-------------------------------------------------------------------------------
  1. Open VS Code.
  2. Go to:  File -> Open Folder...
  3. Select:  Desktop\G2-08_CEP


STEP 3 -- OPEN TERMINAL IN VS CODE
-------------------------------------------------------------------------------
  In VS Code, open the terminal:  Terminal -> New Terminal

STEP 4 -- ENSURE TERMINAL IS RUNNING POWERSHELL 
-------------------------------------------------------------------------------
  Confirm your prompt looks like:
    PS C:\Users\YourName\Desktop\G2-08_CEP>

  If it is not PowerShell, use the terminal dropdown at the top of the
  terminal panel and choose PowerShell.

=============================================
NOTE:
run on g++ , if powershell not working
=============================================

STEP 5 -- COMPILE THE PROJECT
-------------------------------------------------------------------------------
  Inside PowerShell, run the following command:

	g++ -std=c++17 main.cpp User.cpp Member.cpp Admin.cpp Usermanager.cpp 
	Resource.cpp Book.cpp Ebook.cpp BorrowRecord.cpp FineManager.cpp 
	ReportGenerator.cpp LibraryManager.cpp Validator.cpp InputHelper.cpp 
	Notification.cpp AuditLogger.cpp SecurityUtils.cpp AdminAnalytics.cpp
 	AdvancedSearch.cpp BookCopies.cpp BookRating.cpp BorrowExtension.cpp -o app

  Note:
    - The executable "app" ( app.exe ) will be created in the same folder.
    - If g++ is not recognized, install MinGW or use Visual Studio's compiler.


STEP 6 -- RUN THE EXECUTABLE
-------------------------------------------------------------------------------
  After compilation succeeds, run:

    .\app or ./app          


STEP 7 -- START USING THE SYSTEM
-------------------------------------------------------------------------------
  NOTE: On the very first run the system will initialize itself -- it will
  create all required data files (users.txt, resources.txt, etc.) in the
  project folder automatically. No manual file creation is needed.

SYSTEM INITIALIZATION:
    
	1.First-time setup requires Super Admin registration.

	---------------------------------------
  	Admin Access Code :  LibraryAdmin2026!
	---------------------------------------
	Demo Credentials:
	---------------------------------------
  	Demo Username     :  RumaisaWaseem_
  	Demo Password     :  Neduet@52
 	---------------------------------------

    	2. Log in using the Super Admin account.
    	3. Add resources (books / eBooks) and members.
    	4. Start borrowing and returning operations.

===============================================================================
  3. DATA FILES  (Persistent Storage)
===============================================================================

The application uses and updates these files inside the project folder:
 
  ===========================+================================================
  FILE                       |  DESCRIPTION
  ===========================+================================================
  users.txt                  |  Registered members and admins
                             |  (auto-loaded/saved)
  ---------------------------+------------------------------------------------
  resources.txt              |  Books and eBooks catalogue
                             |  (auto-created/loaded)
  ---------------------------+------------------------------------------------
  borrow_records.txt         |  Borrow/return transactions history
  ---------------------------+------------------------------------------------
  notifications.txt          |  Notification storage (system messages)
  ---------------------------+------------------------------------------------
  reservations.txt           |  Reservation records
  ---------------------------+------------------------------------------------
  waitlist.txt               |  Waitlist queue storage
  ---------------------------+------------------------------------------------
  audit_log.txt              |  Admin activity logs
  ===========================+================================================
 
  Optional data folders (created only when you use these features):
 
  ===========================+================================================
  FOLDER                     |  DESCRIPTION
  ===========================+================================================
  ratings/                   |  Stores book ratings and reviews
                             |  (created when ratings are generated)
  ---------------------------+------------------------------------------------
  extensions/                |  Stores borrow extension requests
                             |  (created when extension requests are submitted)
  ===========================+================================================


===============================================================================
  4. SYSTEM FEATURES
===============================================================================

4.1  USER ACCOUNT MANAGEMENT
  - Member and admin registration with duplicate prevention
  - Secure login -- password hashing, salting, and lockout policy
  - Role-based access control (5 admin roles)
  - Membership renewal and account balance top-up
  - Account suspension and reinstatement
  - Profile viewing and management

4.2  RESOURCE MANAGEMENT
  - Add, update, and remove books and eBooks
  - ISBN management, category assignment, and inventory tracking
  - eBook format and digital license management
  - Availability status tracking per copy

4.3  SEARCH & FILTERING
  - Search by title, author, category, ISBN, or resource ID
  - Partial keyword matching and real-time results
  - Availability filtering, category filtering, eBook format filtering

4.4  BORROWING & RETURN
  - Borrow physical books and access digital eBooks
  - Membership-tier-based borrow limits and durations
  - Due-date management, overdue detection, and borrow history
  - Borrow extension requests (up to 14 days, admin approval required)
  - Automatic fine calculation (Rs. 10 / overdue day) on return
  - Damage fee application by admin

4.5  RESERVATION & WAITLIST
  - Reserve available physical resources
  - Automatic FIFO waitlist queue per resource
  - Priority access for reserved users; notification on availability

4.6  FINE MANAGEMENT
  - Automatic overdue fine calculation and tracking
  - Fine payment via member account balance
  - Damage fee application by admin
  - Auto-suspension when unpaid fines exceed Rs. 10,000

4.7  NOTIFICATION SYSTEM
  - Borrow, return, reservation, fine, and extension notifications
  - Admin announcements and system alerts
  - Persistent storage with read/unread tracking

4.8  REPORTS & ANALYTICS
  - Member, resource, fine, overdue, and borrowing history reports
  - Inventory reports and book popularity analytics
  - Member activity, resource utilization, and category trend analysis

4.9  BOOK RATINGS & REVIEWS
  - Rate books 1-5 stars with written reviews (one review per member)
  - View aggregated ratings; admin review moderation

4.10 SECURITY
  - Password hashing with salt; secure admin registration code
  - Login attempt limitation with lockout enforcement
  - Role-based access control and comprehensive input validation
  - Audit logging of all admin actions


===============================================================================
  5. SYSTEM WORKFLOW
===============================================================================

  Step 1 :  Program starts and loads all stored data files.
  Step 2 :  User registers or logs into the system.
  Step 3 :  System validates credentials and identifies user role.
  Step 4 :  Member or Admin dashboard is displayed.
  Step 5 :  Users search, borrow, reserve, or manage resources.
  Step 6 :  Borrow records, fines, and notifications are generated.
  Step 7 :  Reports and analytics are updated.
  Step 8 :  System saves all updated records before termination.


===============================================================================
  6. ADMIN ROLE HIERARCHY
===============================================================================

  Role                Permissions
  ----------------    ---------------------------------------------------------
  Super Admin         Complete system access -- manage admins, members,
                      resources, reports, analytics, and system-level ops
  Standard Admin      Process borrow/return requests; view reports and member
                      activities; monitor operations
  Librarian           Add, update, and remove resources; manage inventory
  Notification Admin  Send announcements; manage and broadcast notifications
  Fine Manager        Manage fines; verify payments; monitor balances;
                      handle overdue penalties


===============================================================================
  7. MEMBERSHIP TIERS & FEE STRUCTURE
===============================================================================

  Tier        Borrow Limit    Borrow Period    Annual Fee
  ---------   ------------    -------------    ----------
  Silver      2 Books         14 Days          Rs.  500
  Gold        5 Books         21 Days          Rs. 1,000
  Platinum    10 Books        30 Days          Rs. 2,000

  Fine & Fee Structure:
  ---------------------
  Overdue Fine             :  Rs. 10 per overdue day
  Damage Fee               :  Applied by admin based on damage severity
  Borrow Extension         :  Up to 14 days; admin approval; once per borrow
  Auto-Suspension Trigger  :  Unpaid fines exceeding Rs. 10,000

 
===============================================================================
  8. OOP CONCEPTS IMPLEMENTED
===============================================================================
 
  ======================+======================================================
  CONCEPT               |  IMPLEMENTATION & LOCATION
  ======================+======================================================
  Encapsulation         |  Private data members with public getters/setters
                        |  in User, Member, Admin, Resource, Book, Ebook
                        |  (User.h, Member.h, Admin.h, Resource.h, Book.h,
                        |   Ebook.h)
  ----------------------+------------------------------------------------------
  Inheritance           |  User --> Member, Admin
                        |  Resource --> Book, Ebook
                        |  (defined in respective .h / .cpp files)
  ----------------------+------------------------------------------------------
  Polymorphism          |  Virtual displayDetails() overridden in Book and
                        |  Ebook; LibraryManager operates on Resource*
                        |  collections at runtime
  ----------------------+------------------------------------------------------
  Abstraction           |  Resource is an abstract base class with pure
                        |  virtual methods; clients interact only through
                        |  the Resource interface  (Resource.h)
  ----------------------+------------------------------------------------------
  Composition /         |  LibraryManager owns collections of resources and
  Aggregation           |  borrow records; BookRating and BorrowExtension
                        |  each own and manage their related data
  ----------------------+------------------------------------------------------
  Exception Handling    |  Custom exception hierarchy in SYSTEMEXCEPTIONS.h;
                        |  try/catch blocks throughout LibraryManager,
                        |  UserManager, and main.cpp workflows
  ----------------------+------------------------------------------------------
  File Handling /       |  UserManager  --> users.txt
  Persistence           |  LibraryManager --> resources & transaction history
                        |  AuditLogger  --> audit_log.txt
  ----------------------+------------------------------------------------------
  STL Containers        |  vector<Resource*>, vector<BorrowRecord*> in
                        |  LibraryManager; map/set used for search and
                        |  rating indexing
  ----------------------+------------------------------------------------------
  Static Members /      |  Validator  : static input validation helpers
  Methods               |  SecurityUtils : static hashing helpers
                        |  Domain classes : static ID generation patterns
  ----------------------+------------------------------------------------------
  Design Patterns       |  Singleton-style : UserManager, LibraryManager
                        |  Factory        : deserialize() rebuilds objects
                        |  Observer-like  : Notification events on workflows
                        |  Template Method: ReportGenerator base + specialised
                        |                  report output subclasses
  ======================+======================================================
 


===============================================================================
  9. MAJOR CLASSES & RESPONSIBILITIES
===============================================================================

  User  (User.cpp / User.h)
    Base class for all system users. Stores authentication data and supports
    the polymorphism hierarchy.
    - login()           : verifies credentials, enforces lockout policy
    - logout()          : ends active session
    - displayProfile()  : shows complete profile info
    - updateProfile()   : updates user info
    - changePassword()  : securely changes password

  Member  (Member.cpp / Member.h)
    Represents a normal library member.
    - borrowResource() / returnResource() / reserveResource()
    - requestBorrowExtension()
    - payFine() / addFunds()
    - viewBorrowHistory()

  Admin  (Admin.cpp / Admin.h)
    Represents administrators with role-based permissions.
    - addResource() / updateResource() / removeResource()
    - suspendMember() / reinstateMember()
    - generateReports()
    - processExtensionRequest()

  Resource  (Resource.cpp / Resource.h)
    Abstract base class for all library resources.
    - displayDetails()       : pure virtual; overridden in Book and Ebook
    - updateAvailability()   : tracks borrowing status

  LibraryManager  (LibraryManager.cpp)
    Central orchestrator. Owns all resource collections, borrow records,
    fines, reservations, waitlists, notifications, and data persistence.

  Supporting Classes:
    FineManager       -- fine calculation, tracking, and payment
    ReportGenerator   -- all report and analytics generation
    Notification      -- notification creation, storage, read/unread state
    AuditLogger       -- admin activity logging to audit_log.txt
    SecurityUtils     -- password hashing, salt, admin code verification
    Validator         -- input validation helpers
    InputHelper       -- safe input routines
    BookRating        -- rating and review management
    BorrowExtension   -- extension request lifecycle and admin approval
    AdvancedSearch    -- search helpers
    AdminAnalytics    -- admin analytics
    BookCopies        -- per-copy physical inventory management


===============================================================================
  10. SOURCE FILE INVENTORY
===============================================================================

  Total: 22 header files (.h)  |  23 source files (.cpp)

  Header Files (.h)        Source Files (.cpp)
  ---------------------    ---------------------
  Admin.h                  Admin.cpp
  AdminAnalytics.h         AdminAnalytics.cpp
  AdvancedSearch.h         AdvancedSearch.cpp
  AuditLogger.h            AuditLogger.cpp
  Book.h                   Book.cpp
  BookCopies.h             BookCopies.cpp
  BookRating.h             BookRating.cpp
  BorrowExtension.h        BorrowExtension.cpp
  BorrowRecord.h           BorrowRecord.cpp
  Ebook.h                  Ebook.cpp
  FineManager.h            FineManager.cpp
  InputHelper.h            InputHelper.cpp
  LibraryManager.h         LibraryManager.cpp
  Member.h                 Member.cpp
  Notification.h           Notification.cpp
  ReportGenerator.h        ReportGenerator.cpp
  Resource.h               Resource.cpp
  SecurityUtils.h          SecurityUtils.cpp
  SYSTEMEXCEPTIONS.h       User.cpp
  User.h                   Usermanager.cpp
  Usermanager.h            Validator.cpp
  Validator.h              main.cpp


===============================================================================
  11. IMPLEMENTATION STATUS
===============================================================================

  Fully Implemented:
    User registration and authentication (members and admins)
    Resource management (books and eBooks)
    Borrow/return processing with automatic fine calculation
    Role-based access control (5 admin roles)
    Waitlist and reservation system
    Notification system with persistence and read/unread tracking
    Reporting and analytics
    Rating and review system
    Borrow extension requests and approval workflow
    Audit logging of admin actions
    Data persistence across sessions
    Input validation and exception handling


===============================================================================
  12. EDGE CASES & SOLUTIONS
===============================================================================

   1) Duplicate username registration
      Where   : Usermanager.cpp
      Problem : Username already exists.
      Solution: userExists() check; throws ValidationException.

   2) Admin registration without secret code
      Where   : Usermanager.cpp, SecurityUtils.cpp
      Problem : Unauthorized admin role creation.
      Solution: 3-attempt limit; verifyAdminAccessCode(); throws
                OperationCancelledException.

   3) More than one Super Admin
      Where   : Usermanager.cpp
      Problem : Super Admin must be unique.
      Solution: superAdminExists() guard; throws ValidationException.

   4) Brute-force login attempts
      Where   : User.cpp, main.cpp
      Problem : Multiple failed logins should lock the account.
      Solution: incrementFailedLogin(); isLocked() blocks access
                until lockout expires.

   5) Invalid IDs or menu input
      Where   : Validator.cpp, InputHelper
      Problem : Garbage inputs crash logic.
      Solution: Static validators; ValidationException on bad input;
                user may retry.

   6) Due/return date ordering errors
      Where   : BorrowRecord.cpp
      Problem : Due date before issue date, or return date in future.
      Solution: createRecord() and setReturnDate() throw
                InvalidDateException on invalid ordering.

   7) Double-borrowing the same resource
      Where   : LibraryManager.cpp
      Problem : Member tries to borrow an already-active borrow.
      Solution: findRecord() checks for existing active borrow;
                throws ResourceStateException.

   8) Borrowing with a locked account
      Where   : LibraryManager.cpp
      Problem : Locked members must be blocked.
      Solution: isLocked() guard; throws AuthorizationException.

   9) Borrowing with expired membership
      Where   : LibraryManager.cpp, Member.cpp
      Problem : Expired membership should prevent borrowing.
      Solution: isMembershipValid() guard; throws AuthorizationException.

  10) Borrow limit exceeded
      Where   : LibraryManager.cpp, Member.cpp
      Problem : Member must not borrow beyond tier maximum.
      Solution: canBorrow() guard; throws ResourceStateException
                with tier limit details.

  11) Borrowing with outstanding fines
      Where   : LibraryManager.cpp
      Problem : Unpaid fines must block new borrows.
      Solution: getBalance() < 0 check; throws ResourceStateException.

  12) Borrowing a resource reserved by another member
      Where   : LibraryManager.cpp
      Problem : Reservation conflicts must be enforced.
      Solution: isReserved() + holder mismatch; throws
                ResourceStateException.

  13) Unavailable resource or exhausted eBook licenses
      Where   : Resource classes, LibraryManager.cpp
      Problem : Physical book borrowed or eBook licenses gone.
      Solution: checkAvailability() must be true; throws
                ResourceStateException.

  14) Fair waitlist ordering on return
      Where   : LibraryManager.cpp
      Problem : Users must be served in FIFO order.
      Solution: FIFO queue per resource; first user notified with
                BOOK_AVAILABLE notification on return.

  15) Reserving an unavailable resource
      Where   : LibraryManager.cpp
      Problem : Cannot reserve what is not available.
      Solution: checkAvailability() false throws ResourceStateException,
                directs user to waitlist.

  16) Reserving a digital resource
      Where   : LibraryManager.cpp
      Problem : Digital items cannot be reserved.
      Solution: isDigital() check; throws ResourceStateException.

  17) Double reservation by same user
      Where   : LibraryManager.cpp
      Problem : Same user tries to reserve again.
      Solution: Holder match check; throws ResourceStateException.

  18) Late fine calculation on return
      Where   : LibraryManager.cpp -> returnResource()
      Problem : Must calculate fine only for physical overdue borrows.
      Solution: Parses due/return dates; lateFee = daysOverdue x 10;
                eBooks excluded via dynamic_cast<EBook*>.

  19) Damage fee validation
      Where   : LibraryManager.cpp -> applyDamageFee()
      Problem : Fee must be positive and member must exist.
      Solution: damageFee <= 0 throws ValidationException; member lookup
                throws NotFoundException if missing; FINE_APPLIED
                notification sent on success.

  20) Returning a non-borrowed resource
      Where   : LibraryManager.cpp
      Problem : No active borrow record to close.
      Solution: Active record search by userID + resourceID;
                throws NotFoundException if missing.

  21) Rating out of range or duplicate review
      Where   : LibraryManager.cpp, BookRating.cpp
      Problem : Invalid rating or member reviewing twice.
      Solution: rating < 1 || > 5 throws ValidationException;
                duplicate reviewer throws runtime_error.

  22) Extension days out of range (1-14)
      Where   : LibraryManager.cpp, BorrowExtension
      Problem : Extension days invalid or limit exceeded.
      Solution: daysToExtend < 1 || > 14 throws ValidationException;
                module enforces per-borrow extension limits.

  23) Extension pending admin approval lifecycle
      Where   : BorrowExtension.cpp, main.cpp
      Problem : Members submit requests and must be notified of decision.
      Solution: requestExtension() creates pending request; notifications
                sent on admin approve or reject action.

  24) Corrupted file lines on load
      Where   : LibraryManager.cpp, Usermanager.cpp
      Problem : Malformed data must not crash the system.
      Solution: try/catch around each line parse; malformed rows
                skipped silently.

  25) File not openable on save
      Where   : LibraryManager.cpp, Usermanager.cpp
      Problem : Failed saves must not go unnoticed.
      Solution: if (!file.is_open()) throws runtime_error.

  26) Notification persistence and read tracking
      Where   : LibraryManager.cpp, Notification.h
      Problem : Notifications must persist and track read state.
      Solution: saveNotifications() serializes to notifications.txt;
                markNotificationAsRead() flips read state per member.

  27) Deleting a currently borrowed resource
      Where   : LibraryManager.cpp
      Problem : Deleting an active borrow breaks history integrity.
      Solution: checkAvailability() false throws ResourceStateException;
                cannot delete while resource is borrowed.

  28) Memory cleanup on exit
      Where   : LibraryManager destructor
      Problem : Heap-allocated objects must be freed on shutdown.
      Solution: Destructor saves all data then deletes all records,
                resources, notifications, ratings, and managers.

==============================================================================
   13. PROJECT STRUCTURE
==============================================================================

LibraryManagementSystem/
¦
+-- main.cpp
¦
+-- User.h / User.cpp
+-- Member.h / Member.cpp
+-- Admin.h / Admin.cpp
+-- Resource.h / Resource.cpp
+-- Book.h / Book.cpp
+-- Ebook.h / Ebook.cpp
+-- BorrowRecord.h / BorrowRecord.cpp
+-- FineManager.h / FineManager.cpp
+-- UserManager.h / UserManager.cpp
+-- LibraryManager.h / LibraryManager.cpp
+-- Notification.h / Notification.cpp
+-- ReportGenerator.h / ReportGenerator.cpp
+-- SecurityUtils.h / SecurityUtils.cpp
+-- Validator.h / Validator.cpp
+-- InputHelper.h / InputHelper.cpp
+-- AuditLogger.h / AuditLogger.cpp
+-- AdminAnalytics.h / AdminAnalytics.cpp
+-- AdvancedSearch.h / AdvancedSearch.cpp
+-- BookRating.h / BookRating.cpp
+-- BorrowExtension.h / BorrowExtension.cpp
¦
+-- users.txt
+-- resources.txt
+-- borrow_records.txt
+-- notifications.txt
+-- reservations.txt
+-- waitlist.txt
+-- audit_log.txt
¦
+-- README.txt

===============================================================================
               END OF README  --  Library Management System
===============================================================================