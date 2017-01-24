function res = contrast_full(gray, ratio),

height = size(gray, 1);
width = size(gray, 2);
xstart = int32(width * ratio(1) + 1);
xend = int32(width * ratio(2));
ystart = int32(height * ratio(3) + 1);
yend = int32(height * ratio(4));

base = gray(ystart+1:yend-1, xstart+1:xend-1);
mu = gray(ystart+1:yend-1, xstart:xend-2);
mr = gray(ystart+2:yend, xstart+1:xend-1);
md = gray(ystart+1:yend-1, xstart+2:xend);
ml = gray(ystart:yend-2, xstart+1:xend-1);

res = abs(mu-base)+abs(mr-base)+abs(md-base)+abs(ml-base);

