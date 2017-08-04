#include "drawingUtils.hpp"

void drawPointCloudCollections(boost::shared_ptr<lcm::LCM> &lcm, int index, Eigen::Isometry3d& pose, pcl::PointCloud<pcl::PointXYZRGB>& pcl_cloud, long long int utime, std::string pc_name_root)
{
  pronto_vis* pc_vis;
  pc_vis = new pronto_vis( lcm->getUnderlyingLCM() );

  int reset = 1;
  // Names
  std::stringstream pc_name;
  pc_name << pc_name_root << " ";
  pc_name << to_string(index);
  std::stringstream frame_name;
  frame_name << "PC Frame ";
  frame_name << to_string(index);
  // Indexes
  int pc_index = index+100;
  int frame_index = index;
  // Color
  srand (time(NULL));
  float r = (rand() % 101)/100.0;
  float g = (rand() % 101)/100.0;
  float b = (rand() % 101)/100.0;
  // obj: id name type reset
  // pts: id name type reset objcoll usergb rgb
  pc_vis->obj_cfg_list.push_back( obj_cfg(frame_index,frame_name.str().c_str(),5,reset) );
  pc_vis->ptcld_cfg_list.push_back( ptcld_cfg(pc_index,pc_name.str().c_str(),1,1,0,-1,{0.0,0.0,0.0})); // use RGB as randomly generated by viewer (uergb=-1)

  // Publish cloud and frame:
  Isometry3dTime poseT = Isometry3dTime(utime, pose);
  pc_vis->pose_to_lcm_from_list(frame_index, poseT);
  pc_vis->ptcld_to_lcm_from_list(pc_index, pcl_cloud, utime, utime);
}

void drawPointCloudNormalsCollections(boost::shared_ptr<lcm::LCM> &lcm, int index, Eigen::Isometry3d& pose, pcl::PointCloud<pcl::PointXYZRGBNormal>& pcl_cloud, long long int utime, std::string pc_name_root)
{
  pronto_vis* pc_vis;
  pc_vis = new pronto_vis( lcm->getUnderlyingLCM() );
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_tmp (new pcl::PointCloud<pcl::PointXYZRGB>);

  int reset = 1;
  // Names
  std::stringstream pc_name;
  pc_name << pc_name_root << " ";
  pc_name << to_string(index);
  std::stringstream frame_name;
  frame_name << "PC Frame ";
  frame_name << to_string(index);
  // Indexes
  int pc_index = index+1;
  int frame_index = index;

  Isometry3dTime poseT = Isometry3dTime(utime, pose);
  pc_vis->pose_to_lcm_from_list(frame_index, poseT);

  // Create normals list
  std::vector<Eigen::Vector3d> normals_list;
  for (size_t i = 0; i < pcl_cloud.points.size(); i++){
    Eigen::Vector4f p = pcl_cloud.points[i].getNormalVector4fMap ();
    normals_list.push_back ( Eigen::Vector3d( p[0],p[1],p[2] ) ) ;
  }
  pcl::copyPointCloud(pcl_cloud, *cloud_tmp);

  // obj: id name type reset
  // pts: id name type reset objcoll usergb rgb
  obj_cfg oconfig = obj_cfg(frame_index,frame_name.str().c_str(),5,reset);
  pc_vis->pose_to_lcm(oconfig, poseT);
  ptcld_cfg pconfig = ptcld_cfg(pc_index, pc_name.str().c_str(), 1, 1, frame_index, 0, {0.1,0.7,0.1});
  pc_vis->ptcld_to_lcm(pconfig, *cloud_tmp, normals_list, 0, 0);
}

// NOTE: Visualization using lcmgl shows (sometimes) imperfections
// in clouds (random lines). The clouds are actually correct, but lcm cannot manage
// so many points and introduces mistakes in visualization.
void drawPointCloudLCMGL(bot_lcmgl_t *lcmgl, pcl::PointCloud<pcl::PointXYZRGB>& pcl_cloud)
{
  vector<Eigen::Vector3f> point_cloud;
  int cloud_size = pcl_cloud.points.size();
  float n_supp_points = 70000.0; // max number of floats supported by lcm channel
  int step = round(cloud_size/n_supp_points);
  point_cloud.resize(n_supp_points);
  int j = 0;
  for (int point = 0; point < cloud_size; point=point+step)
  {
    if (j < point_cloud.size())
      point_cloud.at(j) << pcl_cloud.points[point].x, pcl_cloud.points[point].y, pcl_cloud.points[point].z; 
    j++;
  }
    
  drawPointCloudLCMGL(lcmgl, point_cloud);
}

void drawPointCloudLCMGL(bot_lcmgl_t *lcmgl, std::vector<Vector3f> point_cloud)
{
  bot_lcmgl_point_size(lcmgl, 1);
  srand (time(NULL));
  float r = (rand() % 101)/100.0;
  float g = (rand() % 101)/100.0;
  float b = (rand() % 101)/100.0;
  bot_lcmgl_color3f(lcmgl, r, g, b);
  bot_lcmgl_begin(lcmgl, LCMGL_POINTS);
  for (Vector3f point : point_cloud)
  {
    bot_lcmgl_vertex3f(lcmgl, point(0), point(1), point(2));
  }
  bot_lcmgl_end(lcmgl);
  bot_lcmgl_switch_buffer(lcmgl);
}

void drawFrameCollections(boost::shared_ptr<lcm::LCM> &lcm, int index, Eigen::Isometry3d& pose, long long int utime, std::string frame_name_root)
{
  pronto_vis* pc_vis;
  pc_vis = new pronto_vis( lcm->getUnderlyingLCM() );

  int reset =1;
  // Name
  std::stringstream frame_name;
  frame_name << frame_name_root << " ";
  frame_name << to_string(index);
  // obj: id name type reset
  pc_vis->obj_cfg_list.push_back( obj_cfg(index,frame_name.str().c_str(),5,reset) );

  // Publish frame:
  Isometry3dTime poseT = Isometry3dTime(utime, pose);
  pc_vis->pose_to_lcm_from_list(index, poseT);
}

void drawFrameLCMGL(bot_lcmgl_t *lcmgl)
{
	// draw world origin frame (0,0,0)
  bot_lcmgl_point_size(lcmgl, 3);
  bot_lcmgl_begin(lcmgl, LCMGL_LINES);

  //x-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(1, 0, 0);
  lcmglVertex3f(1, 0, 0);
  lcmglVertex3f(0, 0, 0);
  lcmglEnd();

  //y-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(0, 1, 0);
  lcmglVertex3f(0, 1, 0);
  lcmglVertex3f(0, 0, 0);
  lcmglEnd();

  //z-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(0, 0, 1);
  lcmglVertex3f(0, 0, 1);
  lcmglVertex3f(0, 0, 0);
  lcmglEnd();

  bot_lcmgl_end(lcmgl);
  bot_lcmgl_switch_buffer(lcmgl);
}

void drawFrameLCMGL(bot_lcmgl_t *lcmgl, Eigen::Isometry3d transform)
{
  Eigen::Vector3d origin, x_tip, y_tip, z_tip;
  Eigen::Matrix3d rot = transform.rotation();
  //cout << "rot: " << endl << rot << endl;
  origin << transform.translation();
  x_tip << rot(0,0)+origin(0), rot(1,0)+origin(1), rot(2,0)+origin(2);
  y_tip << rot(0,1)+origin(0), rot(1,1)+origin(1), rot(2,1)+origin(2);
  z_tip << rot(0,2)+origin(0), rot(1,2)+origin(1), rot(2,2)+origin(2); 

  drawFrameLCMGL(lcmgl, origin, x_tip, y_tip, z_tip);
}

void drawFrameLCMGL(bot_lcmgl_t *lcmgl, Eigen::Vector3d origin,
	           Eigen::Vector3d x_tip, Eigen::Vector3d y_tip, Eigen::Vector3d z_tip)
{
  bot_lcmgl_point_size(lcmgl, 3);
  bot_lcmgl_begin(lcmgl, LCMGL_LINES);

  //x-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(1, 0, 0);
  lcmglVertex3d(x_tip(0), x_tip(1), x_tip(2));
  lcmglVertex3d(origin(0), origin(1), origin(2));
  lcmglEnd();

  //y-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(0, 1, 0);
  lcmglVertex3d(y_tip(0), y_tip(1), y_tip(2));
  lcmglVertex3d(origin(0), origin(1), origin(2));
  lcmglEnd();

  //z-axis
  lcmglBegin(LCMGL_LINES);
  lcmglColor3f(0, 0, 1);
  lcmglVertex3d(z_tip(0), z_tip(1), z_tip(2));
  lcmglVertex3d(origin(0), origin(1), origin(2));
  lcmglEnd();

  bot_lcmgl_end(lcmgl);
  bot_lcmgl_switch_buffer(lcmgl);
}


void draw3dLine(bot_lcmgl_t *lcmgl, Vector3d start, Vector3d end)
{
  bot_lcmgl_begin(lcmgl, LCMGL_LINES);
  bot_lcmgl_vertex3f(lcmgl, start[0], start[1], start[2]);
  bot_lcmgl_vertex3f(lcmgl, end[0], end[1], end[2]);
  bot_lcmgl_end(lcmgl);
}

void draw3dLine(bot_lcmgl_t *lcmgl, double start_x, double start_y, double start_z, double end_x, double end_y, double end_z)
{
  bot_lcmgl_begin(lcmgl, LCMGL_LINES);
  bot_lcmgl_vertex3f(lcmgl, start_x, start_y, start_z);
  bot_lcmgl_vertex3f(lcmgl, end_x, end_y, end_z);
  bot_lcmgl_end(lcmgl);
}

void HSVtoRGB( float &r, float &g, float &b, float h, float s, float v )
{
  int i;
  float f, p, q, t;

  if( s == 0 ) {
    // achromatic (grey)
    r = g = b = v;
    return;
  }

  h /= 60;      // sector 0 to 5
  i = floor( h );
  f = h - i;      // factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );

  switch( i ) {
    case 0:
      r = v;
      g = t;
      b = p;
    break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
    r = t;
      g = p;
    b = v;
    break;
    default:    // case 5:
      r = v;
      g = p;
      b = q;
    break;
  }
}

void publishOctreeToLCM(boost::shared_ptr<lcm::LCM> &lcm, octomap::ColorOcTree* tree, string octree_channel){
  octomap_raw_t msg;
  msg.utime = bot_timestamp_now();

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      msg.transform[i][j] = 0;
    }
    msg.transform[i][i] = 1;
  }

  std::stringstream datastream;
  tree->write(datastream);
  std::string datastring = datastream.str();
  msg.data = (uint8_t *) datastring.c_str();
  msg.length = datastring.size();

  octomap_raw_t_publish(lcm->getUnderlyingLCM(), octree_channel.c_str(), &msg);
}
