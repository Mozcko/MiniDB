-- MiniDB Test Script
-- Este script prueba las funcionalidades clave de la base de datos.

-- 1. Crear una tabla para empleados con tipos de datos específicos.
CREATE TABLE empleados (id INTEGER, puesto TEXT, salario INTEGER);

-- 2. Insertar datos válidos. Estos deberían funcionar.
INSERT INTO empleados VALUES (101, 'Desarrollador', 50000);
INSERT INTO empleados VALUES (102, 'Diseñador', 45000);
INSERT INTO empleados VALUES (103, 'Gerente', 70000);

-- 3. Intentar insertar datos con tipos incorrectos.
-- La siguiente línea debería fallar porque 'mucho' no es un INTEGER para el salario.
INSERT INTO empleados VALUES (104, 'Analista', 'mucho');

-- 4. Seleccionar todos los datos para verificar las inserciones.
-- Deberíamos ver solo a los tres primeros empleados. El cuarto no debería aparecer.
SELECT * FROM empleados;

-- 5. Realizar consultas más específicas con WHERE.
SELECT puesto FROM empleados WHERE salario = 70000;
SELECT id, salario FROM empleados WHERE puesto = 'Desarrollador';
