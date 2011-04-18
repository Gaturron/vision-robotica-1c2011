function p = rectificacion(x)
    image = imread(x);
    [tamx, tamy] = size(image);

    %+Mostramos la imagen
    imshow(image);
    hold on;
    
    %+Obtenemos los puntos
    p1 = getMouseXY('rx-');
    hold on;
    p2 = getMouseXY('rx-');
    hold on;
    p3 = getMouseXY('rx-');
    
    p4 = getMouseXY('gx-');
    hold on;
    p5 = getMouseXY('gx-');
    hold on;
    p6 = getMouseXY('gx-');
    
    %+Calculamos las distancias
    %me imagino que la onda es asi (nose que pasa sino los alineas asi)
    %  p1 ----------- p2 -------- p3
    a_prima = p2 - p1;
    a_prima = norm(a_prima);
    b_prima = p3 - p2;
    b_prima = norm(b_prima);
    %  p4 ----------- p5 -------- p6
    a_prima_prima = p5 - p4;
    a_prima_prima = norm(a_prima_prima);
    b_prima_prima = p6 - p5;
    b_prima_prima = norm(b_prima_prima);
    %si, los nombres son una bostaaa :P
   
    %+Calcular P inf 1
    val = (a_prima * (a_prima + b_prima))/(a_prima - b_prima);
    p_inf1 = [val, 1]
    %+Calcular P inf 2
    val = (a_prima_prima * (a_prima_prima + b_prima_prima))/(a_prima_prima - b_prima_prima);
    p_inf2 = [val, 1]
    
    %+Cross de P inf 1 y P inf 2
    %aca le agregue un 1 pero no estoy seguro
    L = cross([p_inf1 1], [p_inf2 1]);
        
    %+Tenemos L: hallar H
    H = [1 0 0 ; 0 1 0; L];
    %+Ccalcular inv(H)
    H_inv = inv(H);
    
    %+For que llene la imagen
    image_rec = zeros(tamx, tamy);
    for x = 1:tamx
        for y = 1:tamy
            temp = H_inv * [x; y; 0];
            temp = round(temp);
            image_rec(temp(1), temp(2)) = image(x,y);
        end
    end
    
    imshow(image_rec)
end