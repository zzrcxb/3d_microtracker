function plot_lens_distance(endindex, path),

res = ones(1,endindex);

for i = 1:1:endindex,
    res(i) = contrast_full(strcat(path, int2str(i), '.jpg'), [0.1, 0.9, 0.1, 0.9]);
end

x = 1:1:endindex;

size(res)
size(x)

plot(x, res)