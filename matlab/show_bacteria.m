function show_bacteria(path, step_len, starttime),

v = VideoReader(path);
v.CurrentTime = starttime;

vidFrame = readFrame(v);
data = rgb2gray(vidFrame);

[y, x] = find(data > max(max(data)) - 1);
data = uint8(data > (max(max(data)) - 2));

posx = find(sum(data, 1) == max(sum(data, 1)));
posy = find(sum(data, 2) == max(sum(data, 2)));

posx = posx(1);
posy = posy(1);

x = x - posx;
y = y - posy;

x = int32(x);
y = int32(y);

judge = uint8(x.^2 + y.^2 <= 100);
total = sum(judge);
posx = posx +  uint32(sum(x .* int32(judge)) / total);
posy = posy + uint32(sum(y .* int32(judge)) / total);

focus_range = ones(1, 4);

if (posx - step_len <= 0)
    focus_range(1) = 0;
else
    focus_range(1) = posx - step_len;
end

if posy - step_len <= 0,
    focus_range(3) = 0;
else
    focus_range(3) = posy - step_len;
end

if posx + step_len > size(vidFrame, 2),
    focus_range(2) = 0;
else
    focus_range(2) = posx + step_len;
end

if posy + step_len > size(vidFrame, 1),
    focus_range(4) = 0;
else
    focus_range(4) = posy + step_len;
end

focus_range = int32(focus_range);

currAxes = axes;
while hasFrame(v),
    vidFrame = readFrame(v);
    
    data = rgb2gray(vidFrame);
    
    globalmax = max(max(data));

    focus_window = data(focus_range(3):focus_range(4), focus_range(1): focus_range(2));

    [y, x] = find(focus_window > max(max(focus_window)) - 1);
    resx = mean2(x) - 1 + focus_range(1);
    resy = mean2(y) - 1 + focus_range(3);
    contrast_value = sum(sum(contrast_full(focus_window, [0, 1, 0, 1]) .^ 2));

    if (resx - step_len <= 0)
        focus_range(1) = 0;
    else
        focus_range(1) = resx - step_len;
    end
    
    if resy - step_len <= 0,
        focus_range(3) = 0;
    else
        focus_range(3) = resy - step_len;
    end
    
    if resx + step_len > size(vidFrame, 2),
        focus_range(2) = 0;
    else
        focus_range(2) = resx + step_len;
    end
    
    if resy + step_len > size(vidFrame, 1),
        focus_range(4) = 0;
    else
        focus_range(4) = resy + step_len;
    end

    resframe = insertShape(data, 'circle', [resx, resy, 15], 'LineWidth', 5, 'Color', 'red');
    resframe = insertShape(resframe, 'rectangle', [focus_range(1), focus_range(3), 2 * step_len, 2 * step_len], 'LineWidth', 2, 'Color', 'yellow');
    
    image(resframe, 'Parent', currAxes);
    currAxes.Visible = 'off';

    pause(1/v.FrameRate);
end

function ImageClickCallBack(objectHandle, eventData),
    axesHandle  = get(objectHandle,'Parent');
    coordinates = get(axesHandle,'CurrentPoint'); 
    coordinates = coordinates(1,1:2);
    message     = sprintf('x: %.1f , y: %.1f',coordinates (1) ,coordinates (2));
    helpdlg(message);
end

end