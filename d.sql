-- remove all records from the tables client, employee, project, and works_on

DELETE FROM client;
DELETE FROM employee;
DELETE FROM project;
DELETE FROM works_on;

-- remove all foreign keys from the tables client, employee, project, and works_on
ALTER TABLE client DROP FOREIGN KEY client_ibfk_1;
ALTER TABLE employee DROP FOREIGN KEY employee_ibfk_1;
ALTER TABLE project DROP FOREIGN KEY project_ibfk_1;
ALTER TABLE works_on DROP FOREIGN KEY works_on_ibfk_1;
ALTER TABLE works_on DROP FOREIGN KEY works_on_ibfk_2;

-- drop the tables client, employee, project, and works_on
DROP TABLE client;
DROP TABLE employee;
DROP TABLE project;
DROP TABLE works_on;

show tables;