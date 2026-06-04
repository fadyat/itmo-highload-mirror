# Лабораторная работа 0

[Google Drive](https://drive.google.com/drive/folders/16Xk6DTgTgCqjzua5a2spJca_vsZ7RRT0)

## PostgreSQL

### Схема

```
students (id, full_name, group_id, enrollment_year)
groups (id, group_name, faculty_id)
faculties (id, faculty_name)
courses (id, course_name, credits)
enrollments (id, student_id, course_id, grade)
lecturers (id, full_name, department)
teaching (id, lecturer_id, course_id, semester)
```

### Задания

1. Составить запрос для получения списка студентов с указанием их учебных групп и факультетов.
2. Составить запрос для отображения всех курсов и количества студентов, записанных на каждый из них.
3. Составить запрос для получения списка студентов, у которых средний балл выше определённого значения.

---

## MongoDB

### Схема

```
products: { _id, name, category_id, price, stock, supplier_id }
categories: { _id, category_name, parent_id }
customers: { _id, full_name, email, registration_date }
orders: { _id, customer_id, order_date, total_amount }
order_items: { _id, order_id, product_id, quantity, unit_price }
suppliers: { _id, name, country }
shipments: { _id, order_id, shipment_date, status }
```

### Задания

1. Получить список товаров с указанием их категорий.
2. Определить количество товаров в каждой категории.
3. Найти клиентов, совершивших заказы на сумму выше определённого значения.

---

## Neo4j

### Схема

```
Warehouse (id, name, city)
Store (id, name, city)
Supplier (id, name, country)
Product (id, name, category, price)

Отношения:
  (Warehouse)-[:ROUTE {distance, time}]->(Store)
  (Supplier)-[:SUPPLIES]->(Warehouse)
  (Warehouse)-[:STOCKS {quantity}]->(Product)
  (Store)-[:ORDERS {quantity}]->(Product)
```

### Задания

1. Получить список всех складов и городов, в которых они расположены.
2. Определить все маршруты между складами и магазинами с указанием расстояния.
3. Найти все магазины, которые заказывают определённый товар.

---

## Spark SQL

### Схема

```
customers (id, full_name, city, registration_date)
accounts (id, customer_id, account_type, balance, branch_id)
branches (id, branch_name, city)
transactions (id, account_id, amount, transaction_date, transaction_type)
loans (id, customer_id, loan_amount, interest_rate, start_date, status)
employees (id, full_name, branch_id, position, salary)
```

### Задания

1. Получить список клиентов и городов, в которых они зарегистрированы.
2. Определить количество счетов каждого типа.
3. Найти клиентов, у которых средний баланс по счетам превышает определённое значение.
