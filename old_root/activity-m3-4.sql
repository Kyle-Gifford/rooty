SELECT Invoices.InvoiceID, Customers.CustomerName, Customers.City, Customers.State, CURDATE() AS Date, Invoices.TotalDue
FROM Customers
INNER JOIN Invoices ON Customers.CustomerID = Invoices.CustomerID
WHERE Invoices.InvoiceID = 3;