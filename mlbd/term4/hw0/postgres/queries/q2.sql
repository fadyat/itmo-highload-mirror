select c.course_name, count(distinct e.student_id) as students_enrolled
from courses c
         left join enrollments e on c.id = e.course_id
group by c.id;
