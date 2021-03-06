clear; clc;

% Flagfield definitions (multiple boundaries)
FLUID           = 0;
NO_SLIP         = 1;
FREE_SLIP       = 2;
OUTFLOW         = 3;

MOVING_WALL     = 10;

INFLOW          = 30;
INFLOW_1        = 31;
INFLOW_2        = 32;
INFLOW_3        = 33;
INFLOW_4        = 34;
INFLOW_5        = 35;

PRESSURE_IN     = 50;
PRESSURE_IN_1   = 51;
PRESSURE_IN_2   = 52;
PRESSURE_IN_3   = 53;
PRESSURE_IN_4   = 54;
PRESSURE_IN_5   = 55;

PARALLEL_BOUNDARY = 7;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set these:
x = 8;                          % size parameter
max_flag = 55;                  % maximum number that appears in flags
tables = true;                  % show tables
control_room = true;            % show the control room
library = true;                 % show the library
slides = true;                  % show the slides
HS1 = true;                     % show the Horsaal 1 cut

NORTH_DOOR      = INFLOW_1;     % main entrance
EAST_DOOR       = OUTFLOW;      % to LRZ
CANTINE_DOOR    = INFLOW_2;     % next to the cantine
SOUTH_DOOR      = OUTFLOW;      % next to the library, to LRZ
WEST_DOOR       = INFLOW_3;     % back entrance
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Dimensions for the hall and the wings
% l=length (x), w=width (y), h=height (z), d=distance
hall_l = 32*x;
hall_w = 4*x;
hall_h = 1;  

wing_l = 2*x;
wing_w = 8*x;
wing_h = 1;
wing_d = 4*x;

% Canvas size
Nx = hall_l;
Ny = wing_w - 1 + hall_w - 1 + wing_w;
Nz = hall_h;

% Initialize the canvas (MI building)
MI = NO_SLIP * ones(Nx, Ny, Nz);

% Construct the main hall
Hall = NO_SLIP * ones(hall_l, hall_w, hall_h);
Hall(2:hall_l-1, 2:hall_w-1, :) = FLUID;

% Add the doors
% North door (main entrance)
x_pos = 1 + 2*x + 1; % East boundary + 2*x + start
y_pos = 1;
door_size = x;
Hall(x_pos:x_pos+door_size-1, y_pos, :) = NORTH_DOOR;

% East door (LRZ)
x_pos = 1;
y_pos = hall_w - 2*x; % hall width - 2*x
door_size = x;
Hall(x_pos, y_pos:y_pos+door_size-1, :) = EAST_DOOR;

% Cantine door
x_pos = 1 + wing_l + 2*(wing_l + wing_d) + wing_l + x;
y_pos = hall_w;
door_size = x;
Hall(x_pos:x_pos+door_size-1, y_pos, :) = CANTINE_DOOR;

% South door (side door, library)
x_pos = hall_l-3*x;
y_pos = hall_w;
door_size = x;
Hall(x_pos:x_pos+door_size-1, y_pos, :) = SOUTH_DOOR;

% West door (back entrance)
x_pos = hall_l;
y_pos = 1 + x + 1;
door_size = x;
Hall(x_pos, y_pos:y_pos+door_size-1, :) = WEST_DOOR;

% Add the tables (at least x=4 is required)
if (tables)
    table_l = x/4;
    table_w = x/2;
    table_d = x;
    
    % North-east set (Rechnerhalle)
    table_start_x = wing_l + wing_d + wing_l + x; 
    table_start_y = 1 + x;
    start_x = table_start_x;
    end_x = start_x + table_l;
    start_y = table_start_y;
    end_y = start_y + table_w;
    Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;

    for i=1:5
        start_x = start_x + table_d;
        end_x = end_x + table_d;
        Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;
    end

    % North-west set (Tree)
    start_x = start_x + wing_l + wing_d;
    end_x = start_x + table_l;

    for i=1:5
        start_x = start_x + table_d;
        end_x = end_x + table_d;
        Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;
    end

    % South set (Cantine)
    start_x = 2*(wing_l + wing_d) + x;
    end_x = start_x + table_l;
    start_y = hall_w -1 -x - table_w + 1;
    end_y = start_y + table_w;
    Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;

    for i=1:10
        start_x = start_x + table_d;
        end_x = end_x + table_d;
        Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;
    end
end

% Control room
if (control_room)
    control_room_size = x/2;
    start_x = x;
    end_x = start_x + control_room_size;
    start_y = 3*x-2;
    end_y = start_y + control_room_size;
    Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;
end

% Library
if (library)
   library_l = 3*x/2;
   library_w = x;
   end_x = hall_l;
   start_x = end_x-library_l;
   end_y = hall_w;
   start_y = end_y-library_w;
   Hall(start_x:end_x, start_y:end_y, :) = NO_SLIP;   
end

% Slides
if (slides)
   slides_l = 6*x/6;
   start_x = 2*x;
   end_x = start_x + x/6;
   for i=-1*start_x:end_x
       j = floor(0.01*i^2);
       pos_x = i+17*x+3;
       pos_y = j+table_start_y-2;
       Hall(pos_x:pos_x+1,pos_y:pos_y+2,:) = NO_SLIP;
   end

end

% Horsaal 1
if (HS1)
   for i=1:x
      for j=1:i
          Hall(x-i+1,j,:) = NO_SLIP;
      end
   end
end


% Put the main hall in place
MI(: , wing_w:wing_w+hall_w-1, : ) = Hall;

% Construct a north wing
WingN = NO_SLIP * ones(wing_l, wing_w, wing_h);
WingN(2:wing_l-1, 2:wing_w-1, :) = FLUID;
% WingN(2:wing_l-1, wing_w, :) = FLUID;
WingN(1:wing_l, wing_w, :) = PARALLEL_BOUNDARY;

% Put the north wings in place
xpos = wing_l + wing_d + 1; % one wing + distance + east boundary
ypos = 1;
MI(xpos:xpos+wing_l-1, ypos:ypos+wing_w-1, :) = WingN; 

for i=2:5
    xpos = xpos + wing_l + wing_d;
    MI(xpos:xpos+wing_l-1, ypos:ypos+wing_w-1, :) = WingN; 
end

% Construct a south wing
WingS = NO_SLIP * ones(wing_l, wing_w, wing_h);
WingS(2:wing_l-1, 2:wing_w-1, :) = FLUID;
% WingS(2:wing_l-1, 1, :) = FLUID; 
WingS(1:wing_l, 1, :) = PARALLEL_BOUNDARY; 

% Put the south wings in place
xpos = wing_l + 1; % one wing + east boundary
ypos = wing_w -1 + hall_w;
MI(xpos:xpos+wing_l-1, ypos:ypos+wing_w-1, :) = WingS; 

for i=2:5
    xpos = xpos + wing_l + wing_d;
    MI(xpos:xpos+wing_l-1, ypos:ypos+wing_w-1, :) = WingS; 
end

% Visualize the domain
image(rot90(10*MI(:,:,1)));
axis equal
xlim([0 Nx + 1]) % x for the graph only
ylim([0 Ny + 1]) % y for the graph only

% Write pgm files

% Write the visualized (disable the parallel boundaries first!)
% full_domain = flipud(rot90(MI));
% f_name = 'pgm/full_domain.pgm';
% f_id = fopen(f_name, 'w');
% fprintf(f_id, '%s\n', ['P3 ', num2str(size(full_domain,2)), ' ', num2str(size(full_domain,1)), ' ', num2str(max_flag)]);
% fclose(f_id);
% dlmwrite('pgm/full_domain.pgm', full_domain, '-append', 'delimiter', ' ')

% Main Hall
n_cpus = 4; % Number of partitions-cpus for the main hall
hall_part_l = hall_l / n_cpus;
if (mod(hall_l, n_cpus) ~= 0)
    printf('Careful! Hall not divisible by number of cpus!')
end
x_start = 1;
x_end = hall_part_l;
y_start = wing_w;
y_end = y_start + hall_w -1;
part_0 = flipud(rot90([MI(x_start:x_end, y_start:y_end, :); PARALLEL_BOUNDARY*ones(1, hall_w, hall_h)]));
f_name = 'pgm/cpu_0.pgm';
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(part_0,2)), ' ', num2str(size(part_0,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite('pgm/cpu_0.pgm', part_0, '-append', 'delimiter', ' ')

file_id = 0;

for i=1:n_cpus-2
    file_id = file_id + 1;
    x_start = x_start + hall_part_l;
    x_end = x_end + hall_part_l;
    clear part_i
    part_i = flipud(rot90([PARALLEL_BOUNDARY*ones(1, hall_w, hall_h); MI(x_start:x_end, y_start:y_end, :); PARALLEL_BOUNDARY*ones(1, hall_w, hall_h)]));
    f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
    f_id = fopen(f_name,'w');
    fprintf(f_id, '%s\n', ['P3 ', num2str(size(part_i,2)), ' ', num2str(size(part_i,1)), ' ', num2str(max_flag)]);
    fclose(f_id);
    dlmwrite(f_name, part_i, '-append', 'delimiter', ' ')
end

file_id = file_id + 1;
x_start = x_start + hall_part_l;
x_end = x_end + hall_part_l;
part_n = flipud(rot90([PARALLEL_BOUNDARY*ones(1, hall_w, hall_h); MI(x_start:x_end, y_start:y_end, :)]));
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(part_n,2)), ' ', num2str(size(part_n,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(['pgm/cpu_',num2str(file_id),'.pgm'], part_n, '-append', 'delimiter', ' ')

% North wings
WingN(1, wing_w, :) = NO_SLIP;
WingN(2:wing_l-1, wing_w, :) = FLUID;
WingN(wing_l, wing_w, :) = NO_SLIP;
WingN_par = flipud(rot90([WingN PARALLEL_BOUNDARY * ones(wing_l, 1, wing_h)]));
file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingN_par,2)), ' ', num2str(size(WingN_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingN_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingN_par,2)), ' ', num2str(size(WingN_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingN_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingN_par,2)), ' ', num2str(size(WingN_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingN_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingN_par,2)), ' ', num2str(size(WingN_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingN_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingN_par,2)), ' ', num2str(size(WingN_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingN_par, '-append', 'delimiter', ' ')

% South wings
WingS(1, 1, :) = NO_SLIP;
WingS(2:wing_l-1, 1, :) = FLUID; 
WingS(wing_l, 1, :) = NO_SLIP;
WingS_par = flipud(rot90([PARALLEL_BOUNDARY * ones(wing_l, 1, wing_h) WingS]));
file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingS_par,2)), ' ', num2str(size(WingS_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name,  WingS_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingS_par,2)), ' ', num2str(size(WingS_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingS_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingS_par,2)), ' ', num2str(size(WingS_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingS_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingS_par,2)), ' ', num2str(size(WingS_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingS_par, '-append', 'delimiter', ' ')

file_id = file_id + 1;
f_name = ['pgm/cpu_',num2str(file_id),'.pgm'];
f_id = fopen(f_name,'w');
fprintf(f_id, '%s\n', ['P3 ', num2str(size(WingS_par,2)), ' ', num2str(size(WingS_par,1)), ' ', num2str(max_flag)]);
fclose(f_id);
dlmwrite(f_name, WingS_par, '-append', 'delimiter', ' ')

