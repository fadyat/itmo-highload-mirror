select s.full_name,
       g.group_name,
       f.faculty_name
from students s
         join groups g on s.group_id = g.id
         join faculties f on g.faculty_id = f.id;
