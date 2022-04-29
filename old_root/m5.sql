-- Getting the Invoice Slip Body
-- Getting the invoice slip body is akin to getting the itemized list of things that were sold on that invoice and each item's associated details.

-- ‼️ Create a final query for your last activity-m3-5.sql. From left to right, the columns that should appear are Products.ProductNumber, Products.ProductName, InvoiceDetails.LineTotal, InvoiceDetails.OrderQty, and InvoiceDetails.UnitPrice. The InvoiceID we want the details for is 3. Your results should be similar to the below. Nothing is ordered or grouped.

select Products.ProductNumber, Products.ProductName, InvoiceDetails.LineTotal, InvoiceDetails.OrderQty, InvoiceDetails.UnitPrice
from Products
INNER JOIN InvoiceDetails on Products.ProductNumber = InvoiceDetails.ProductNumber
where InvoiceDetails.InvoiceID = 3;