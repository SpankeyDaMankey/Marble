#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

#include "texture.h"
#include "marble_element.h"
#include "marble_structure.h"

class hit_record;

class material {
  public:
    virtual ~material() = default;

    virtual color emitted(double u, double v, const point3& p) const {
        return color(0,0,0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }

    virtual color subsurface_scatter(double u, double v, const point3& p, const hit_record& rec) {
      return color(0,0,0);
    }
};

class lambertian : public material {
  public:
    lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    shared_ptr<texture> tex;
};

class marble : public material {
  public:
  int count = 0;
    marble(const color& albedo, int size) : tex(make_shared<solid_color>(albedo)), size(size), ms(size) 
    {
      initialize_marble();
    }
    marble(shared_ptr<texture> tex, int size) : tex(tex), size(size), ms(size) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

    color subsurface_scatter(double u, double v, const point3& p, const hit_record& rec) override
    {
        //cout << "doing subsurface scatter: " << ++count << endl;
        int xi, yi, zi;
        double xd, yd, zd;

        if(rec.side_name == "front")
        {
            xd = 1 - v;
            yd = u;
            zd = 0;

            xi = static_cast<int>(xd * size);
            yi = static_cast<int>(yd * size);
            zi = 0;
            //cout << "Front" << endl;   
        }
        else if(rec.side_name == "right")
        {
            xd = 1 - v;
            yd = size - 1;
            zd = u;

            xi = static_cast<int>(xd * size);
            yi = size - 1;
            zi = static_cast<int>(zd * size);
            //cout << "Right" << endl; 
        }
        else if(rec.side_name == "back")
        {
            xd = 1 - v;
            yd = 1 - u;
            zd = size - 1;

            xi = static_cast<int>(xd * size);
            yi = static_cast<int>(yd * size);
            zi = size - 1;
            //cout << "Back" << endl;
        }
        else if(rec.side_name == "left")
        {
            xd = 1 - v;
            yd = 0;
            zd = 1 - u;

            xi = static_cast<int>(xd * size);
            yi = 0;
            zi = static_cast<int>(zd * size);
            cout << "Left" << endl;  
        }
        else if(rec.side_name == "top")
        {
            xd = 0;
            yd = u;
            zd = v;

            xi = 0;
            yi = static_cast<int>(yd * size);
            zi = static_cast<int>(zd * size);
            //cout << "Top" << endl;
        }
        else if(rec.side_name == "bottom")
        {
            xd = size - 1;
            yd = u;
            zd = 1 - v;

            xi = size - 1;
            yi = static_cast<int>(yd * size);
            zi = static_cast<int>(zd * size);
            //cout << "Bottom" << endl;
        }
        else if(rec.side_name == "quad_default")
        {
            return color(0,0,0);
        }

        if(xi == size)
        {
            xi -= 1;
        }

        if(yi == size)
        {
            yi -= 1;
        }

        if(zi == size)
        {
            zi -= 1;
        }
      
      return ms.getMarble()[xi][yi][zi].getEnergy();

    }

    void initialize_marble()
    {
        ms.Initialize_Energy();
        //ms.Initialize_Half_Density();
        //ms.Initialize_Random_Density();
        ms.Propagate_Energy();
    }


  private:
    shared_ptr<texture> tex;
    int size;
    Marble_Structure ms;
    
};


class metal : public material {
  public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

class diffuse_light : public material {
  public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color& emit) : tex(make_shared<solid_color>(emit)) {}

    color emitted(double u, double v, const point3& p) const override {
        return tex->value(u, v, p);
    }

  private:
    shared_ptr<texture> tex;
};

#endif