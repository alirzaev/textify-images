# Обработка фотографий текста

Программа разработана с использованием материалов из статьи https://habr.com/ru/post/53341/

## Описание алгоритма

В программе используется "метод с делением слоёв" из приведённой выше статьи:

1. Создать копию изображения;

2. Размыть копию по Гауссу (радиус - 100);

3. Свести оба изображения в режиме деления. Подробнее о режиме "деления" (divide): https://docs.gimp.org/en/gimp-concepts-layer-modes.html

Пример обработанной фотографии: [до](sample/before.jpg), [после (цветная)](sample/after_colored.jpg), [после (ч/б)](sample/after_grayscaled.jpg).

## Сборка

Проект собирается штатными средствами Qt.
