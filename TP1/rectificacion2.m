function p = rectificacion2(x)
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
    p3 = getMouseXY('gx-');
    hold on;
    p4 = getMouseXY('gx-');
    hold on;
    
    %imwrite(image, 'in.png');
    
    %+Calculamos con rectas paralelas
    L2 = cross([p1 1], [p2 1]);
    L1 = cross([p3 1], [p4 1]);
    p_inf1 = cross(L1, L2);
    p_inf1 = p_inf1/p_inf1(3)
    
    L3 = cross([p4 1], [p2 1]);
    L4 = cross([p3 1], [p1 1]);
    p_inf2 = cross(L3, L4);
    p_inf2 = p_inf2/p_inf2(3)

    %+Cross de P inf 1 y P inf 2
    L = cross([p_inf1], [p_inf2])
    L = L / norm(L);
        
    %+Tenemos L: hallar H
    H = [1 0 0 ; 0 1 0; L]

    %+For que llene la imagen
    image_rec = zeros(tamx*3, tamy*3);
    image_rec = uint8(image_rec);
    size(image_rec)
    for x = 1:1:tamx*3
        for y = 1:1:tamy*3
            temp = H * [x; y; 1];
            temp = round(temp/temp(3)) ;
            if(1 <= temp(1) && temp(1) <=tamx && 1 <= temp(2) && temp(2) <=tamy)
                image_rec((x+tamx/4), (y+tamy/4)) = image(temp(1),temp(2));
                10
            end
        end
    end

    imshow(image_rec)
    image_rec;
    %imwrite(image_rec, 'out.png');
end
