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

import com.google.drive.samples.dredit.model.State;
import com.google.gson.Gson;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet to check that the current user is authorized and to serve the
 * start page for DrEdit.
 *
 * @author vicfryzel@google.com (Vic Fryzel)
 * @author nivco@google.com (Nicolas Garnier)
 */
@SuppressWarnings("serial")
public class StartPageServlet extends DrEditServlet {
  /**
   * Ensure that the user is authorized, and setup the required values for
   * index.jsp.
   */
  @Override
  public void doGet(HttpServletRequest req, HttpServletResponse resp)
      throws IOException, ServletException {
	// Making sure the code gets processed
    req.setAttribute("client_id", new Gson().toJson(getClientId(req, resp)));
    // Deserialize the state in order to specify some values to the DrEdit
    // JavaScript client below.
    if (req.getParameter("state") != null) {
      State state = new State(req.getParameter("state"));
      if (state.ids != null && state.ids.size() > 0) {
        resp.sendRedirect("/#/edit/" + state.ids.toArray()[0]);
        return;
      } else if (state.parentId != null) {
        resp.sendRedirect("/#/create/" + state.parentId);
        return;
      }
    }
    req.getRequestDispatcher("/WEB-INF/templates/index.jsp").forward(req, resp);
  }
}