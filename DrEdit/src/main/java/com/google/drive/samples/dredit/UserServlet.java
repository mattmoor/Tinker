/*
 * Copyright (c) 2012 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

package com.google.drive.samples.dredit;

import com.google.api.client.auth.oauth2.Credential;
import com.google.api.client.googleapis.json.GoogleJsonResponseException;
import com.google.api.services.oauth2.Oauth2;
import com.google.api.services.oauth2.model.Userinfo;
import com.google.gson.Gson;

import java.io.IOException;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet that returns the profile of the currently logged-in user.
 *
 * @author nivco@google.com (Nicolas Garnier)
 */
@SuppressWarnings("serial")
public class UserServlet extends DrEditServlet {
  /**
   * Returns a JSON representation of the user's profile.
   */
  @Override
  public void doGet(HttpServletRequest req, HttpServletResponse resp)
      throws IOException {
  	Oauth2 service = getOauth2Service(req, resp);
    try {
    	Userinfo about = service.userinfo().get().execute();
      resp.setContentType(JSON_MIMETYPE);
      resp.getWriter().print(new Gson().toJson(about));
    } catch (GoogleJsonResponseException e) {
      if (e.getStatusCode() == 401) {
        // The user has revoked our token or it is otherwise bad.
        // Delete the local copy so that their next page load will recover.
        deleteCredential(req, resp);
        sendError(resp, 401, "Unauthorized");
        return;
      }
    }
  }

  /**
   * Build and return an Oauth2 service object based on given request parameters.
   *
   * @param req Request to use to fetch code parameter or accessToken session
   *            attribute.
   * @param resp HTTP response to use for redirecting for authorization if
   *             needed.
   * @return Drive service object that is ready to make requests, or null if
   *         there was a problem.
   */
  private Oauth2 getOauth2Service(HttpServletRequest req,
      HttpServletResponse resp) {
    Credential credentials = getCredential(req, resp);

    return new Oauth2.Builder(TRANSPORT, JSON_FACTORY, credentials).build();
  }
}