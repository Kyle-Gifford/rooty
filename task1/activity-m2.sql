-- activity 1

CREATE TABLE Customers (
    CustomerID int NOT NULL AUTO_INCREMENT,
    CustomerName varchar(50),
    AddressLine1 varchar(50),
    AddressLine2 varchar(50),
    City varchar(50),
    State varchar(50),
    PostalCode varchar(50),
    YTDPurchases decimal(19, 2),
    PRIMARY KEY (CustomerID)
);

INSERT INTO Customers (CustomerName, AddressLine1, City, State, PostalCode)
VALUES ('Bike World', '60025 Bollinger Canyon Road', 'San Ramon', 'California', '94583'),
('Metro Sports', '482505 Warm Springs Blvd.', 'Fremont', 'California', '94536'),
('Cameron Cycle', '1233 Sprocket Lane', 'San Mateo', 'California', '94402');

select * from Customers;
select CustomerID, CustomerName from Customers where PostalCode = '94536';

-- activity 3

CREATE TABLE TermsCode (
    TermsCodeID varchar(50) NOT NULL,
    Description varchar(50),
    PRIMARY KEY (TermsCodeID)
);

INSERT INTO TermsCode (TermsCodeID, Description)
VALUES 
('NET30', 'Payment due in 30 days.'),
('NET15', 'Payment due in 15 days.'),
('210NET30', '2% discount in 10 days Net 30');

CREATE TABLE Invoices (
    InvoiceID int NOT NULL AUTO_INCREMENT,
    CustomerID int,
    InvoiceDate datetime,
    TermsCodeID varchar(50),
    TotalDue decimal(19,2),
    PRIMARY KEY (InvoiceID),
    FOREIGN KEY (CustomerID) REFERENCES Customers(CustomerID),
    FOREIGN KEY (TermsCodeID) REFERENCES TermsCode(TermsCodeID)
);

INSERT INTO Invoices (CustomerID, InvoiceDate, TotalDue, TermsCodeID)
VALUES
(2, '2014-02-07', 2388.98, 'NET30'),
(1, '2014-02-02', 2443.35, '210NET30'),
(1, '2014-02-09', 8752.30, NULL);
