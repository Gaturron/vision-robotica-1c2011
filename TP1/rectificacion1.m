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
    p_prima = [ ((a_prima * (a_prima + b_prima))/(a_prima - b_prima)) 1];
    pendiente_1 = (p3(1)-p1(1))/(p3(2)-p1(2));
    p_inf1 = pendiente_1 * p_prima + p1
    %+Calcular P inf 2
    p_prima_prima = [ ((a_prima_prima * (a_prima_prima + b_prima_prima))/(a_prima_prima - b_prima_prima)) 1];
    pendiente_2 = (p6(1)-p4(1))/(p6(2)-p4(2));
    p_inf2 = pendiente_2 * p_prima_prima + p4

    %+Cross de P inf 1 y P inf 2
    L = cross([p_inf1 1], [p_inf2 1])
    L = L / norm(L);
        
    %+Tenemos L: hallar H
    H = [1 0 0 ; 0 1 0; L]
%     H = inv(H);
%     
%     %+For que llene la imagen
%     image_rec = zeros(tamx*2, tamy*2);
%     size(image_rec)
%     for x = 1:tamx
%         for y = 1:tamy
%             temp = H * [x; y; 1];
%             temp = round(temp/temp(3)) ;
%             if(1 <= temp(1) && temp(1) <=tamx*2 && 1 <= temp(2) && temp(2) <=tamy*2)
%                 image_rec(temp(1), temp(2)) = image(x,y);
%             end
%         end
%     end
    
    %+For que llene la imagen
    image_rec = zeros(tamx*2, tamy*2);
    size(image_rec)
    for x = 1:tamx*2
        for y = 1:tamy*2
            temp = H * [x; y; 1];
            temp = round(temp/temp(3)) ;
            if(1 <= temp(1) && temp(1) <=tamx && 1 <= temp(2) && temp(2) <=tamy)
                image_rec(x, y) = image(temp(1),temp(2));
            end
        end
    end


    imshow(image_rec);
end
