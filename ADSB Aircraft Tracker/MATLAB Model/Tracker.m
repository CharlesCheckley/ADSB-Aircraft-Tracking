classdef Tracker < handle
    %creates an aircraft object
    %   Detailed explanation goes here
    
    properties
        %physical properties
        x = 0; %x position 
        y = 0; %y position
        z = 0; %z position
        tilt = 0; %current tilt angle
        tiltVel = 0; %current tilt velocity
        tiltAccel = 0; %current tilt accel 
        maxTilt = 180; %max tilt angle 
        maxTiltVel = 99; %max tilt veloctiy in degrees/s
        maxTiltAcceleration = 900; %max tilt acceleration degrees/s^2 
        pan = 0; %current pan angle
        panVel = 0; 
        panAccel = 0; 
        maxPan; %max pan angle 
        maxPanVel = 99; %max pan veloctiy in degrees/s
        maxPanAcceleration = 990; %max pan acceleration degrees/s^2 
        
        vfov = 2.845; %vertical field of view in degrees
        hfov = 4.247; %horizontal field of view in degrees
        
        %control demands
        desTilt; %desired tilt angle 
        desTiltRate = 0; %desired tilt velocity 
        desTiltAccel; 
        desPan; 
        desPanRate = 0; 
        desPanAccel; 
                
        
    end
    
    methods
        function obj = Tracker(x,y,z,tilt,pan)
            %Initiates the object
            obj.x = x;
            obj.y = y; 
            obj.z = z; 
            obj.tilt = tilt; 
            obj.pan = pan;  
        end 
        
        function obj = calculate(obj, aircraft)
            %calculates inverse kinematics for pointing the camera at the
            %aircraft
            
            %pan angle
            xDif = aircraft.x - obj.x; 
            yDif = aircraft.y - obj.y; 
            obj.desPan = atan2d(yDif, xDif); 
            
            %tilt angle
            horizontalDif = sqrt((aircraft.y - obj.y)^2 + (aircraft.x - obj.x)^2); 
            zDif = aircraft.z - obj.z; 
            obj.desTilt = atan2d(zDif, horizontalDif);   
        end
        
        function point(obj)
            %sets the camera pan and tilt angles to their desired angle.
            %primarily a debugging function
            obj.pan = obj.desPan; 
            obj.tilt = obj.desTilt; 
        end
        
        function obj = update(obj, time)
            %updates the camera's position. 

            %Update Tilt Axis
            currentTiltRate = obj.desTiltRate; 

            %Calculate Desired Tilt Rate
            obj.desTiltRate = (obj.desTilt-obj.tilt) / time;  

            %Calculate Desired Tilt Accel
            obj.desTiltAccel = (obj.desTiltRate - currentTiltRate) / time; 

            %Calculate actual Tilt Accel
            if abs(obj.desTiltAccel) < obj.maxTiltAcceleration 
                obj.tiltAccel = obj.desTiltAccel; 
            elseif obj.desTiltAccel < 0 
                obj.tiltAccel = -obj.maxTiltAcceleration; 
            else 
                obj.tiltAccel = obj.maxTiltAcceleration; 
            end

            %Calculate acutal tilt velocity 
            obj.tiltVel = obj.tiltVel + obj.tiltAccel*time; 
            
            if abs(obj.tiltVel) < obj.maxTiltVel      
            elseif obj.tiltVel < 0 
                obj.tiltVel = -obj.maxTiltVel; 
            else 
                obj.tiltVel = obj.maxTiltVel; 
            end

            %Calculate tilt position
            obj.tilt = obj.tilt + obj.tiltVel*time;  



            %Update Pan Axis
            currentPanRate = obj.desPanRate; 

            %Calculate Desired Pan Rate
            obj.desPanRate = (obj.desPan-obj.pan) / time;  

            %Calculate Desired Pan Accel
            obj.desPanAccel = (obj.desPanRate - currentPanRate) / time; 

            %Calculate actual Tilt Accel
            if abs(obj.desPanAccel) < obj.maxPanAcceleration 
                obj.panAccel = obj.desPanAccel; 
            elseif obj.desPanAccel < 0 
                obj.panAccel = -obj.maxPanAcceleration; 
            else 
                obj.panAccel = obj.maxPanAcceleration; 
            end

            %Calculate acutal pan velocity 
            obj.panVel = obj.panVel + obj.panAccel*time; 
            
            if abs(obj.panVel) < obj.maxPanVel      
            elseif obj.panVel < 0 
                obj.panVel = -obj.maxPanVel; 
            else 
                obj.panVel = obj.maxPanVel; 
            end

            %Calculate pan position
            obj.pan = obj.pan + obj.panVel*time; 
            
        end
         
        
        function p = view(obj)
             %calculate corners of FOV in distance

            %DH frame assignment in one drive notes 
            %FOV corner coordinates (top left, top right...) in lens base frame
            distanceX = 10000; %random large value  
            deltay = distanceX * tand(obj.vfov/2);
            deltaz = distanceX * tand(obj.hfov/2);


            TL = [distanceX; deltay; -deltaz; 1]; 
            TR = [distanceX; deltay; deltaz; 1]; 
            BL = [distanceX; -deltay; -deltaz; 1]; 
            BR = [distanceX; -deltay; deltaz; 1]; 
            p = [[0;0;0;1], TL, TR, BL , BR]; 

            %transform to camera rig frame 
            L1 = 0; %physical property of the pan/ tilt assembley 
            L2 = 0; %physical property of the pan/ tilt assembley 
            theta1 = deg2rad(obj.pan);
            theta2 = deg2rad(obj.tilt);


            %Transform matrix for the camera's fov to the global frame 
            m = [[cos(theta1)*cos(theta2), -cos(theta1)*sin(theta2),  sin(theta1), L2*cos(theta1)*cos(theta2)]
                [cos(theta2)*sin(theta1), -sin(theta1)*sin(theta2), -cos(theta1), L2*cos(theta2)*sin(theta1)]
                [            sin(theta2),              cos(theta2),            0,        L1 + L2*sin(theta2)]
                [                      0,                        0,            0,                          1]];

            p = m*p; 
            p(end, :) = [];  % deletes last row of matrix p (the 1 )  
        end 
        
    end
end