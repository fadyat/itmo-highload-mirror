CREATE TABLE faculties
(
    id           SERIAL PRIMARY KEY,
    faculty_name VARCHAR(100) NOT NULL
);

CREATE TABLE groups
(
    id         SERIAL PRIMARY KEY,
    group_name VARCHAR(50) NOT NULL,
    faculty_id INT REFERENCES faculties (id)
);

CREATE TABLE students
(
    id              SERIAL PRIMARY KEY,
    full_name       VARCHAR(100) NOT NULL,
    group_id        INT REFERENCES groups (id),
    enrollment_year INT          NOT NULL
);

CREATE TABLE courses
(
    id          SERIAL PRIMARY KEY,
    course_name VARCHAR(100) NOT NULL,
    credits     INT          NOT NULL
);

CREATE TABLE enrollments
(
    id         SERIAL PRIMARY KEY,
    student_id INT REFERENCES students (id),
    course_id  INT REFERENCES courses (id),
    grade      NUMERIC(3, 1)
);

CREATE TABLE lecturers
(
    id         SERIAL PRIMARY KEY,
    full_name  VARCHAR(100) NOT NULL,
    department VARCHAR(100)
);

CREATE TABLE teaching
(
    id          SERIAL PRIMARY KEY,
    lecturer_id INT REFERENCES lecturers (id),
    course_id   INT REFERENCES courses (id),
    semester    VARCHAR(20)
);

INSERT INTO faculties (faculty_name)
VALUES ('Информатика и вычислительная техника'),
       ('Физика'),
       ('Математика');

INSERT INTO groups (group_name, faculty_id)
VALUES ('ИВТ-101', 1),
       ('ИВТ-102', 1),
       ('ФИЗ-201', 2),
       ('МАТ-301', 3);

INSERT INTO students (full_name, group_id, enrollment_year)
VALUES ('Иванов Иван Иванович', 1, 2022),
       ('Петрова Мария Сергеевна', 1, 2022),
       ('Сидоров Алексей Петрович', 2, 2023),
       ('Козлова Анна Дмитриевна', 3, 2021),
       ('Новиков Дмитрий Олегович', 3, 2021),
       ('Морозова Елена Викторовна', 4, 2020),
       ('Волков Артём Николаевич', 2, 2023),
       ('Лебедева Ольга Андреевна', 4, 2020);

INSERT INTO courses (course_name, credits)
VALUES ('Базы данных', 4),
       ('Алгоритмы', 5),
       ('Линейная алгебра', 3),
       ('Квантовая механика', 4),
       ('Операционные системы', 4);

INSERT INTO enrollments (student_id, course_id, grade)
VALUES (1, 1, 4.5),
       (1, 2, 5.0),
       (2, 1, 3.8),
       (2, 2, 4.2),
       (3, 1, 4.9),
       (3, 5, 4.0),
       (4, 4, 3.5),
       (5, 4, 4.7),
       (5, 3, 4.3),
       (6, 3, 5.0),
       (6, 2, 4.8),
       (7, 1, 3.2),
       (7, 5, 3.6),
       (8, 3, 4.1);

INSERT INTO lecturers (full_name, department)
VALUES ('Профессор Смирнов А.В.', 'Кафедра ИВТ'),
       ('Доцент Кузнецова Е.И.', 'Кафедра физики'),
       ('Профессор Попов В.Г.', 'Кафедра математики');

INSERT INTO teaching (lecturer_id, course_id, semester)
VALUES (1, 1, '2024-осень'),
       (1, 5, '2024-осень'),
       (2, 4, '2024-весна'),
       (3, 2, '2024-осень'),
       (3, 3, '2024-весна');
