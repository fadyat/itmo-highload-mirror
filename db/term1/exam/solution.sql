with valid_scores as (
    select
        student_id,
        hw,
        score,
        count(*) over (partition by hw) as total_submissions
    from (
        select student_id, 'hw1' as hw, hw1 as score from scores where hw1 is not null
        union all
        select student_id, 'hw2' as hw, hw2 as score from scores where hw2 is not null
        union all
        select student_id, 'hw3' as hw, hw3 as score from scores where hw3 is not null
    ) s
),
filtered_scores as (
    select student_id, hw, score
    from valid_scores
    where total_submissions >= 5
),
ranked_scores as (
    select student_id, hw
    from (
        select
            student_id,
            hw,
            score,
            rank() over (partition by hw order by score desc) as rnk
        from filtered_scores
    ) s
    where rnk <= 3
),
top_students as (
    select student_id, count(*) as top_hw
    from ranked_scores
    group by student_id
)
select student_id
from top_students
where top_hw >= 2