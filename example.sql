-- MiniDB Test Script
-- Este script prueba las funcionalidades CRUD y las comparaciones numéricas.

-- 1. Crear una tabla para un inventario.
CREATE TABLE inventario (id INTEGER, producto TEXT, cantidad INTEGER);

-- 2. Insertar datos válidos. Estos deberían funcionar.
INSERT INTO inventario VALUES (1, 'Laptop', 10);
INSERT INTO inventario VALUES (2, 'Mouse', 50);
INSERT INTO inventario VALUES (3, 'Teclado', 25);
INSERT INTO inventario VALUES (4, 'Monitor', 15);

-- 3. Mostrar el estado inicial de la tabla.
SELECT * FROM inventario;

-- 4. Probar UPDATE. Aumentar la cantidad de Laptops.
UPDATE inventario SET cantidad = 12 WHERE id = 1;

-- 5. Probar DELETE. Eliminar el producto 'Mouse'.
DELETE FROM inventario WHERE producto = 'Mouse';

-- 6. Probar SELECT con comparaciones numéricas.
-- Seleccionar productos con cantidad menor o igual a 15.
SELECT producto, cantidad FROM inventario WHERE cantidad <= 15;

-- 7. Mostrar el estado final de la tabla para verificar los cambios.
-- Deberíamos ver la cantidad de Laptops actualizada a 12 y el Mouse ya no debería existir.
SELECT * FROM inventario;
