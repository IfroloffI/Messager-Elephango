# Messager-Elephango
Мессенджер на C++ с графическим интерфейсом

# Здесь должно быть ТЗ, но оно пока в docs

Создание Тестовой БД:

### Пользователи
    CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender VARCHAR(255),
    message TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
### Сообщения
    CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender VARCHAR(255),
    message TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
