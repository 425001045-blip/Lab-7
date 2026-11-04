CREATE DATABASE UniversityDB;
USE UniversityDB;

-- 1. Departments Table
CREATE TABLE Departments (
department_id INT PRIMARY KEY AUTO_INCREMENT,
dept_name VARCHAR(100) NOT NULL UNIQUE,
building VARCHAR(50) DEFAULT 'Main Campus');

-- 2. Professors Table
CREATE TABLE Professors (
professor_id INT PRIMARY KEY AUTO_INCREMENT,
first_name VARCHAR(50) NOT NULL,
last_name VARCHAR(50) NOT NULL,
email VARCHAR(100) UNIQUE NOT NULL,
salary DECIMAL(10,2) CHECK (salary >= 20000.00),
department_id INT,
FOREIGN KEY (department_id) REFERENCES Departments(department_id)
);
-- 3. Courses Table
CREATE TABLE Courses (
course_id INT PRIMARY KEY AUTO_INCREMENT,
course_name VARCHAR(100) NOT NULL,
credits INT CHECK (credits > 0 AND credits <= 6),
department_id INT,
FOREIGN KEY (department_id) REFERENCES Departments(department_id)
);
-- 4. Students Table
CREATE TABLE Students (
student_id INT PRIMARY KEY AUTO_INCREMENT,
first_name VARCHAR(50) NOT NULL,
last_name VARCHAR(50) NOT NULL,
city VARCHAR(50) NOT NULL,
enrollment_year INT DEFAULT 2026,
department_id INT,
FOREIGN KEY (department_id) REFERENCES Departments(department_id)
);
-- 5. Enrollments Table
CREATE TABLE Enrollments (
enrollment_id INT PRIMARY KEY AUTO_INCREMENT,
student_id INT NOT NULL,
course_id INT NOT NULL,
score DECIMAL(5,2) CHECK (score >= 0.00 AND score <= 100.00),
FOREIGN KEY (student_id) REFERENCES Students(student_id),
FOREIGN KEY (course_id) REFERENCES Courses(course_id) );

-- Insert Departments
INSERT INTO Departments (dept_name, building) VALUES
('Computer Science', 'Tech Hall'),
('Information Technology', 'Tech Hall'),
('Mathematics', 'Science Building'),
('Business Administration', 'West Wing');

-- Insert Professors
INSERT INTO Professors (first_name, last_name, email, salary, department_id) VALUES
('Alan',' Turing', 'aturing@univ.edu', 85000.00, 1),
('Ada', 'Lovelace', 'alovelace@univ.edu', 92000.00, 1),
('John', 'von Neumann', 'jneumann@univ.edu&' , 78000.00, 3),
('Grace', 'Hopper' , 'ghopper@univ.edu', 88000.00, 2),
('Claude', 'Shannon', 'cshannon@univ.edu', 65000.00, NULL); -- Unassigned dept

-- Insert Courses
INSERT INTO Courses (course_name, credits, department_id) VALUES
('Database Systems', 4, 1),
('Web Development', 3, 2),
('Linear Algebra', 3, 3),
('Data Structures', 4, 1),
('Business Ethics', 2, 4);

-- Insert Students
INSERT INTO Students (first_name, last_name, city, department_id) VALUES
('Juan', 'Santos', 'Manila', 1),
('Maria', 'Reyes', 'Quezon City', 1),
('Angelo', 'Cruz', 'Manila', 2),
('Anne', 'Del Rosario', 'Makati', 3),
('Mark', 'Garcia', 'Quezon City', NULL);  -- Unassigned student

-- Insert Enrollments
INSERT INTO Enrollments (student_id, course_id, score) VALUES
(1, 1, 95.50),
(1, 4, 88.00),
(2, 1, 72.00),
(2, 2, 85.00),
(3, 2, 90.00),
(4, 3, 64.00);

TRUNCATE TABLE Professors;
DROP TABLE Professors; 

SELECT * FROM Professors; 
SELECT * FROM Enrollments;
SELECT * FROM Students;

SELECT SUM(salary) AS Total_Salary_Expense,  
AVG(salary) AS Average_Professor_Salary
FROM Professors;

SELECT MIN(score) AS Lowest_Score, 
       MAX(score) AS Highest_Score
FROM Enrollments;

SELECT City, COUNT(*) AS total_student 
From students 
Group By city
Having Count(*)> 1;

SELECT course_id, AVG(score) AS average_score
FROM Enrollments
GROUP BY course_id
ORDER BY average_score DESC;

CREATE VIEW SimpleStudentView AS
SELECT student_id, first_name, city
FROM Students;

SELECT * FROM SimpleStudentView;

CREATE VIEW ManilaStudentsView AS
SELECT * 
FROM Students
WHERE city = 'Manila';

SELECT * FROM ManilaStudentsView;

SELECT SUM(salary) AS Total_Salary_Expense, 
       AVG(salary) AS Average_Professor_Salary
FROM Professors;

SELECT MIN(score) AS Lowest_Score, 
       MAX(score) AS Highest_Score
FROM Enrollments;

SELECT city, COUNT(*) AS total_students
FROM Students
GROUP BY city
HAVING COUNT(*) > 1;

SELECT course_id, AVG(score) AS average_score
FROM Enrollments
GROUP BY course_id
ORDER BY average_score DESC;