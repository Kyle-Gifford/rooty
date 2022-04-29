-- ‼️ Add a query to your activity-m3-1.sql that selects the columns ProductNumber, ProductName and ListPrice and sorts it by ListPrice in descending order (most expensive first).

select ProductName, ProductNumber, ListPrice
from Products
order by ListPrice DESC;