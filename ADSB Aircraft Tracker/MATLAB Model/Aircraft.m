classdef Aircraft < handle
    %Aircraft Class
   
    properties
        x = 0;
        y = 0;
        z = 0; 
        xvel = 0; 
        yvel = 0;
        zvel = 0; 
    end
    
    methods
        function obj = Aircraft(x,y,z, xvel, yvel, zvel)
            %creates an aircraft object
            obj.x = x;
            obj.y = y; 
            obj.z = z; 
            obj.xvel = xvel; 
            obj.yvel = yvel; 
            obj.zvel = zvel; 
        end
        
        function obj = update(obj, time)
            %calculates the tracker's desired angles for the aircraft
            obj.x = obj.x + obj.xvel*time; 
            obj.y = obj.y + obj.yvel*time; 
            obj.z = obj.z + obj.zvel*time; 

        end
    end
end

