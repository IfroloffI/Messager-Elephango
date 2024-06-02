# Messager-Elephango
Мессенджер на C++ с графическим интерфейсом

# Здесь должно быть ТЗ, но оно пока в docs

## Создание Тестовой БД:
### Пользователи
    CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(255) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL
    );
### Сообщения
    CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender VARCHAR(255),
    message TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
