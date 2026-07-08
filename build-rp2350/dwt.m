fid = fopen('dwt.bin','r'); im4 = fread(fid, [64,inf], 'int16'); fclose(fid);
figure;
imagesc(im4);
colorbar;
colormap 'gray';
title 'pi_jpeg lena_rgb_64.pgm 1 lvl  07/03/26';
