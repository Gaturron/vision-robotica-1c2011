function p = rectificacion(x)
    image = imread(x);
    [tamx, tamy] = size(image);
    imshow(image);
    hold on;
    p = getMouseXY;
end