function p = getMouseXY(color)
    % Espero 1 click del usuario y le asigno las coordenadas.
    [x,y]=ginput(1);
    % Construyo un vector de 2 coordenadas para devolver el resultado.
    p(1) = x(1);
    p(2) = y(1);
    % Dibujo una cruz en el lugar donde el usuario hizo el click.
    plot(p(1),p(2),color);
end 