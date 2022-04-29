-- Gifford, Kyle (ONID: gifforky)
-- Corbin, Joshua (ONID: corbinj)
-- CS_340_400_S2022
-- Group name: Flaskers
-- Project Name: FlyRide

SET foreign_key_checks = 0;

CREATE TABLE Airports (
    airport_ID varchar(3) UNIQUE NOT NULL,
    name varchar(255) NOT NULL,
    PRIMARY KEY (airport_ID)
);
CREATE TABLE Parking_Spaces (
    parking_space_ID int NOT NULL UNIQUE AUTO_INCREMENT,
    parking_lot_space varchar(255) NOT NULL,
    airport_ID varchar(3) NOT NULL,
    PRIMARY KEY (parking_space_ID),
    FOREIGN KEY (`airport_ID`) REFERENCES `Airports` (`airport_ID`)
);

CREATE TABLE Users (
    user_ID int NOT NULL UNIQUE AUTO_INCREMENT,
    first_name varchar(255) NOT NULL,
    last_name varchar(255) NOT NULL,
    email varchar(255) NOT NULL UNIQUE,
    drivers_licence_number varchar(55) NOT NULL UNIQUE,
    PRIMARY KEY (user_ID)
);

CREATE TABLE Cars (
    car_ID varchar(55) UNIQUE NOT NULL,
    available BIT NOT NULL,
    make varchar(255) NOT NULL,
    model varchar(255) NOT NULL,
    color varchar(55) NOT NULL,
    return_by_date datetime,
    owner_id int NOT NULL,
    current_parking_id int,
    PRIMARY KEY (car_ID),
    FOREIGN KEY (owner_id) REFERENCES Users(user_ID),
    FOREIGN KEY (current_parking_id) REFERENCES Parking_Spaces(parking_space_ID)
);

CREATE TABLE Rental_Orders (
    rental_order_ID int NOT NULL UNIQUE AUTO_INCREMENT,
    airport_ID varchar(3) NOT NULL,
    renter_ID int NOT NULL,
    PRIMARY KEY(rental_order_ID),
    FOREIGN KEY (`airport_ID`) REFERENCES `Airports` (`airport_ID`),
    FOREIGN KEY (`renter_ID`) REFERENCES `users` (`user_ID`)
);

CREATE TABLE Rental_Orders_Details (
    rental_order_ID int NOT NULL,
    car_ID varchar(55) NOT NULL,
    PRIMARY KEY (rental_order_ID, car_ID),
    FOREIGN KEY (`rental_order_ID`) REFERENCES `Rental_Orders` (`rental_order_ID`),
    FOREIGN KEY (`car_ID`) REFERENCES `Cars` (`car_ID`)
);

INSERT INTO Airports (airport_ID, name)
VALUES
('BOS', 'Boston'),
('DFW', 'Dallas-Fort Worth'),
('ORD', 'Chicago'),
('JFK', 'New York');

INSERT INTO Users (first_name, last_name, email, drivers_licence_number)
VALUES
('Kyle', 'Gifford', 'kyle@crazymail.com', '123456789'),
('Joshua', 'Corbin', 'josh@coolguy.com', '987654321'),
('John', 'Doe', 'a@b.com', 'D4772334'),
('Jane', 'Doe', 'r@c.com', 'D4772337'),
('Mary', 'Doe', 'm@does.com', 'D4772338'),
('Bob', 'Doe', 'b@doe.com', 'D4772339'),
('Sally', 'Doe', 's@doe.com', 'D4772340');

INSERT INTO Cars (car_ID, available, make, model, color, return_by_date, owner_id, current_parking_id)
VALUES
('KCN 4887', 0, 'Honda', 'Accord', 'Black', '2022-06-01', '1', NULL),
('XVD 9979', 0, 'Honda', 'Fit', 'Yellow', NULL, '2', NULL),
('NCY 1198', 1, 'Honda', 'Civic', 'Red', '2022-05-05', '1', 1),
('JSG 815', 1, 'Ford', 'Focus', 'Blue', '2022-05-07', '6', 30),
('MGB 346', 0, 'Toyota', 'Camry', 'Green', NULL, '5', NULL),
('WSB 713', 1, 'Tesla', 'Model S', 'White', '2022-05-03', '3', 31),
('JXE 260', 0, 'Tesla', 'Model X', 'Black', '2022-05-04', '4', NULL),
('BMJ 224', 1, 'Tesla', 'Model 3', 'Red', NULL, '5', 32),
('PBE 341', 0, 'Tesla', 'Model Y', 'Blue', '2022-05-04', '6', NULL),
('DBU 625', 1, 'Tesla', 'Model Z', 'Yellow', '2022-05-06', '7', 33);

INSERT INTO Parking_Spaces (parking_lot_space, airport_ID)
VALUES
('D5', 'BOS'),
('D6', 'BOS'),
('D7', 'BOS'),
('D8', 'BOS'),
('D9', 'BOS'),
('D10', 'BOS'),
('D11', 'BOS'),
('D12', 'BOS'),
('D13', 'BOS'),
('D14', 'BOS'),
('D15', 'BOS'),
('D16', 'BOS'),
('D17', 'BOS'),
('D18', 'BOS'),
('D19', 'BOS'),
('D20', 'BOS'),
('D21', 'BOS'),
('D22', 'BOS'),
('D23', 'BOS'),
('D24', 'BOS'),
('D25', 'BOS'),
('D26', 'BOS'),
('D27', 'BOS'),
('D28', 'BOS'),
('D29', 'BOS'),
('D30', 'BOS'),
('75', 'DFW'),
('76', 'DFW'),
('77', 'DFW'),
('78', 'DFW'),
('79', 'DFW'),
('80', 'DFW'),
('81', 'DFW'),
('82', 'DFW'),
('83', 'DFW'),
('84', 'DFW'),
('85', 'DFW'),
('86', 'DFW'),
('87', 'DFW'),
('88', 'DFW'),
('89', 'DFW'),
('90', 'DFW'),
('91', 'DFW'),
('92', 'DFW'),
('93', 'DFW'),
('94', 'DFW'),
('95', 'DFW'),
('96', 'DFW'),
('97', 'DFW'),
('98', 'DFW'),
('99', 'DFW'),
('100', 'DFW'),
('125E', 'ORD'),
('125W', 'ORD'),
('126E', 'ORD'),
('126W', 'ORD'),
('127E', 'ORD'),
('127W', 'ORD'),
('128E', 'ORD'),
('128W', 'ORD'),
('129E', 'ORD'),
('129W', 'ORD'),
('130E', 'ORD'),
('130W', 'ORD'),
('131E', 'ORD'),
('131W', 'ORD'),
('132E', 'ORD'),
('132W', 'ORD'),
('133E', 'ORD'),
('133W', 'ORD'),
('134E', 'ORD'),
('134W', 'ORD'),
('135E', 'ORD'),
('135W', 'ORD'),
('136E', 'ORD'),
('136W', 'ORD'),
('137E', 'ORD'),
('137W', 'ORD'),
('N10', 'JFK'),
('N11', 'JFK'),
('N12', 'JFK'),
('N13', 'JFK'),
('N14', 'JFK'),
('N15', 'JFK'),
('N16', 'JFK'),
('N17', 'JFK'),
('N18', 'JFK'),
('N19', 'JFK'),
('N20', 'JFK'),
('N21', 'JFK'),
('N22', 'JFK'),
('N23', 'JFK'),
('N24', 'JFK'),
('N25', 'JFK'),
('N26', 'JFK'),
('N27', 'JFK'),
('N28', 'JFK'),
('N29', 'JFK'),
('N30', 'JFK'),
('N31', 'JFK'),
('N32', 'JFK'),
('N33', 'JFK'),
('N34', 'JFK');

-- a few random users with random user_ID between 1-7, with random airport_ID
INSERT INTO Rental_Orders (airport_ID, renter_ID)
VALUES
('BOS', 1),
('DFW', 2);

-- use previous info to populate
INSERT INTO Rental_Orders_Details (rental_order_ID, car_ID)
VALUES
(1, 'KCN 4887'),
(2, 'JXE 260'),
(2, 'PBE 341');

SET foreign_key_checks = 1;