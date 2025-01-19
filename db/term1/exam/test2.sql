drop table if exists scores;

create table scores (
    student_id int primary key,
    hw1 int,
    hw2 int,
    hw3 int
);

insert into
    scores (student_id, hw1, hw2, hw3)
values
    (1, 10, 10, null),
    (2, 10, 10, 10),
    (3, 10, null, 10),
    (4, 10, 10, 10),
    (5, 10, 10, 10),
    (6, 10, 10, 10);
