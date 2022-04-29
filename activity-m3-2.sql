--‼️ Add a query to your activity-m3-2.sql that selects the columns InvoiceDetails.InvoiceID, Products.ProductName and InvoiceDetails.UnitPrice and sorts it in ASCending order by UnitPrice where the InvoiceID = 3.

SELECT InvoiceDetails.InvoiceID, Products.ProductName, InvoiceDetails.UnitPrice
FROM InvoiceDetails
INNER JOIN Products ON InvoiceDetails.ProductNumber = Products.ProductNumber
WHERE InvoiceDetails.InvoiceID = 3
GROUP BY Products.ProductName
ORDER BY InvoiceDetails.UnitPrice ASC;