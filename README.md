# ndn_repo_client
This is a c++ client of the ndn-python-repo, and it follows the protocol of ndn-repo<br />
https://ndn-python-repo.readthedocs.io/en/latest/src/readme.html<br />

Start server side:<br />
ndn-python-repo<br />

Build Insturctions:<br />
./waf configure --with-tests --with-examples<br />
sudo ./waf<br />
sudo ./waf install<br />

Run the examples:<br />
export NDN_LOG=ndn_repo_client.*=TRACE:ndn.*=TRACE:examples.*=TRACE<br />
ndn-repo-client-main<br />
