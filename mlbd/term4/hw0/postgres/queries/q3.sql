with averages as (select s.full_name, avg(e.grade) as avg_grade
                  from students s
                           join enrollments e on s.id = e.student_id
                  group by s.id, s.full_name)
select *
from averages
where avg_grade > 4.1;
