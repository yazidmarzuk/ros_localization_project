#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>


ros::ServiceClient client;



void marzuk_drv_rbt(float lin_x, float ang_z)
{

  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  client.call(srv);
}


void process_image_callback(const sensor_msgs::Image I)
{

  int marzukstrt = I.data.size() / 3;
  int marzukend = I.data.size() * 2 / 3;

  int marzuk_no_of_wht_pxls = 0;
  int marzuk_x_pos_sum = 0;


  for (int i=marzukstrt; i+2<marzukend; i+=3) {

    int marzuk_red_chnl = I.data[i];
    int marzuk_grn_chnl = I.data[i+1];
    int marzuk_blu_chnl = I.data[i+2];

    if (marzuk_red_chnl == 255 && marzuk_grn_chnl == 255 && marzuk_blu_chnl == 255)
    {
      int marzuk_x_pos = (i % (I.width * 3)) / 3;
      marzuk_x_pos_sum += marzuk_x_pos;
      marzuk_no_of_wht_pxls += 1;
    }
  }

  if (marzuk_no_of_wht_pxls == 0)
  {
    marzuk_drv_rbt(0.0, 0.0);
  }
  else
  {
    int marzuk_mean_x_pos = marzuk_x_pos_sum / marzuk_no_of_wht_pxls;
    if (marzuk_mean_x_pos < I.width / 3)
    {
      marzuk_drv_rbt(0.5, 0.5);
    }
    else if (marzuk_mean_x_pos > I.width * 2 / 3)
    {
      marzuk_drv_rbt(0.5, -0.5);
    }
    else
    {
      marzuk_drv_rbt(0.5, 0.0);
    }
  }
}

int main(int argc, char** argv)
{

  ros::init(argc, argv, "process_image");
  ros::NodeHandle n;


  client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");



  ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);


  ros::spin();

  return 0;
}
