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
    (1, 10, null, null),
    (2, null, 10, null),
    (3, 10, null, 10),
    (4, null, 10, null),
    (5, 10, null, 10),
    (6, null, 10, null);
