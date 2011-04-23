function p = rectificacion(x)
    image = imread(x);
    [tamx, tamy] = size(image);
    
    %+Mostramos la imagen
    imshow(image);
    hold on;
    
    %+Obtenemos los puntos
    p1 = getMouseXY('rx-')
    hold on;
    p2 = getMouseXY('rx-')
    hold on;
    p3 = getMouseXY('rx-')
    
    p4 = getMouseXY('gx-');
    hold on;
    p5 = getMouseXY('gx-');
    hold on;
    p6 = getMouseXY('gx-');
    
    %imwrite(image, 'in.png');
    
    %calculo las rectas a partir de los puntos dados
    L1 = cross([p1 1], [p2 1])
    L2 = cross([p2 1], [p3 1])
    %L1 y L2 son ortogonales
    
    L3 = cross([p4 1], [p5 1])
    L4 = cross([p5 1], [p6 1])
    %L3 y L4 son ortogonales
    
    %ahora queremos calcular (l1'm1',l1'm2'+ l2'm1' ,l2'm2')s = 0
    %dos rectas l y m son ortogonales si l'*C*_inf*m' entonces A sera una
    %matriz de 2x3 donde cada fila representa dicha ecuacion para cada par 
    %de rectas dados
    
    %Estas dos restricciones representan l'*m = 0 ya que son ortogonales
    
    R1 = [(L1(1)*L2(1)) (L1(2)*L2(2)) (L1(3)*L2(3))]; %restriccion 1
    R2 = [(L3(1)*L4(1)) (L3(2)*L4(2)) (L3(3)*L4(3))]; %restriccion 2
    
    A = [R1;R2]
    
    A_prima = [R1(1) R1(2); R2(1) R2(2)]
    b = [R1(3);R2(3)]
     
    calculoS = (inv(A_prima)*(-b))
    S1 = calculoS(1)
    S2 = calculoS(2)
    
    S = [S1 S2; S2 1]
    
    K = chol(S)
    
    H = [K(1) K(2) 0;K(3) K(4) 0;0 0 1]
    
    %para hacer esto lo escribimos como [A|b]s = 0
    

%     %+For que llene la imagen
     image_rec = zeros(tamx*2, tamy*2);
     image_rec = uint8(image_rec);

     for x = 1:1:tamx*2
       for y = 1:1:tamy*2
           temp = H * [x; y; 1];
           temp = round(temp/temp(3)) ;
           if(1 <= temp(1) && temp(1) <=tamx && 1 <= temp(2) && temp(2) <=tamy)
               image_rec((x + tamx/4), (y + tamy/4)) = image(temp(1),temp(2));
           end
       end
    end
     
     
     %     %size(image_rec)
%     %for x = 1:1:tamx*3
%     %    for y = 1:1:tamy*3
%     %        temp = H * [x; y; 1];
%     %        temp = round(temp/temp(3)) ;
%     %        if(1 <= temp(1) && temp(1) <=tamx && 1 <= temp(2) && temp(2) <=tamy)
%     %            image_rec((x + tamx/4), (y + tamy/4)) = image(temp(1),temp(2));
%     %        end
%     %    end
%     %end
%     for x = 1:1:tamx%filas
%         for y = 1:1:tamy %columnas
%             temp = H * [x; y; 1];
%             temp = round(temp/temp(3));
%             if((-tamx < temp(1)) && (temp(1) <=tamx) && (-tamy < temp(2)) && (temp(2) <=tamy))
%                 image_rec(temp(1) + tamx,temp(2) + tamy) = image(x, y);
%             end
%         end
%     end

    imshow(image_rec)
    %image_rec;
    %imwrite(image_rec, 'out.png');
end